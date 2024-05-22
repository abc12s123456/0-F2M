/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
 
#ifndef _PWM_H_
#define _PWM_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


#define PWM_DC_MAX           10000     //PWM占空比最大值


typedef enum
{
    PWM_Mode_Continue,       //持续输出
    PWM_Mode_Pulse,          //脉冲输出
}PWM_Mode_Enum;


typedef enum
{
    PWM_OMode_Single,        //单通道输出
    PWM_OMode_Comple,        //互补输出
}PWM_OMode_Enum;


typedef struct
{
    u32 Pin_P : 16;
    u32 Pin_N : 16;
    
    /* 使用timer驱动时的通道 */
    u32 CH_P : 8;
    u32 CH_N : 8;
    
    u32 DC : 16;             //占空比
    u32 Frequency;           //频率，单位：Hz
    
    u32 Mode : 2;            //工作模式，参照PWM_Mode_Enum
    u32 OMode : 2;           //输出模式，参照PWM_OMode_Enum
    
    u32 IT_EN : 1;           //PWM中断使能标识
    
    u32 : 27;
}PWM_Config_Type;


typedef struct
{
    FW_Device_Type Device;
    
    PWM_Config_Type Config;
    
    u32 State : 1;           //运行状态，0：停止，1：输出
    u32 : 31;
}PWM_Type;


typedef struct
{
    void (*Init)(PWM_Type *dev);
    
    void (*Set_Frequency)(PWM_Type *dev, u32 freq);
    u32  (*Get_Frequency)(PWM_Type *dev);
    
    void (*Set_DC)(PWM_Type *dev, u16 dc);
    u16  (*Get_DC)(PWM_Type *dev);
    
    void (*CTL)(PWM_Type *dev, u8 state);
}PWM_Driver_Type;


void PWM_Init(PWM_Type *dev);

void PWM_Set_Frequency(PWM_Type *dev, u32 freq);
u32  PWM_Get_Frequency(PWM_Type *dev);

void PWM_Set_DC(PWM_Type *dev, u16 dc);
u16  PWM_Get_DC(PWM_Type *dev);

void PWM_Set_PulseNum(PWM_Type *dev, u32 num);

void PWM_CTL(PWM_Type *dev, u8 state);


#ifdef __cplusplus
}
#endif

#endif

