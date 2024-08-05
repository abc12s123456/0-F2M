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
#ifndef _FW_TIMER_H_
#define _FW_TIMER_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "pwm.h"
#include "qencoder.h"


/* 定时器工作模式 */
typedef enum
{
    FW_TIM_Mode_Timing = 1,       //定时
    FW_TIM_Mode_IC,               //外部输入捕获
    FW_TIM_Mode_PWM,              //PWM输出
    FW_TIM_Mode_Encoder,          //正交编码器
    FW_TIM_Mode_IRCC,             //内部时钟校准(Internal clock calibration)
}FW_TIM_Mode_Enum;


/* 定时器时基单位 */
typedef enum
{
    FW_TIM_Unit_Us,               //
    FW_TIM_Unit_Ms,               //
    FW_TIM_Unit_Ns,               //
}FW_TIM_Unit_Enum;


/* 定时器中断触发模式 */
typedef enum
{
    FW_TIM_IT_Update = 1,         //更新中断（溢出中断）
	FW_TIM_IT_CC,                 //捕获中断
	FW_TIM_IT_COM,                //
	FW_TIM_IT_Trigger,            //触发中断
	FW_TIM_IT_Break,              //刹车中断
}FW_TIM_IT_Enum;


/* 定时器物理通道 */
typedef enum
{
    FW_TIM_CH0  = 0x00,
	FW_TIM_CH1  = 0x01,
	FW_TIM_CH2  = 0x02,
	FW_TIM_CH3  = 0x03,
	FW_TIM_CH4  = 0x04,
	FW_TIM_CH5  = 0x05,
    
    FW_TIM_CH0N = 0x10,
    FW_TIM_CH1N = 0x11,
    FW_TIM_CH2N = 0x12,
    FW_TIM_CH3N = 0x13,
    FW_TIM_CH4N = 0x14,
    FW_TIM_CH5N = 0x15,
    
    FW_TIM_CH_ETI   = 0x20,
    FW_TIM_CH_BRKIN = 0x21,
}FW_TIM_CH_Enum;


//typedef struct
//{
//    u16 P_Pin;               //输出IO
//    u16 N_Pin;               //互补输出IO
//    
//    u32 P_CH : 8;            //输出通道
//    u32 N_CH : 8;            //互补输出通道
//    
//    u32 DC : 16;             //占空比
//}PWM_Config_Type;


//typedef struct
//{
//    u32 A_Pin : 16;               //输入IO
//    u32 B_Pin : 16;
//    
//    u32 A_CH : 8;            //输入通道
//    u32 B_CH : 8;
//    
//    u32 A_VL : 1;            //触发电平
//    u32 B_VL : 1;
//    
//    u32 : 14;
//    
//    u32 Max;                 //编码最大计数值
//}Encoder_Config_Type;


typedef struct
{
    FW_Device_Type Device;
    
    u32 Clock;               //时钟频率
    u32 Timing_Time;         //定时时间，单位：Unit
    
    u32 Mode : 4;            //工作模式，参照FW_TIM_Mode_Enum
    u32 Unit : 4;            //时基单位，参照FW_TIM_Unit_Enum
    
    u32 EN : 1;              //定时器控制标识
    u32 IT_EN : 1;           //中断控制标识
    
    u32 : 22;
    
    /* 中断服务函数 */
    void (*IH_CB)(void *pdata);
    void *IH_Pdata;
    
    /* 底层端口 */
    __RO_ void *TIMx;
}FW_TIM_Type;


typedef struct
{
    void (*DeInit)(FW_TIM_Type *dev);
    
    u32  (*Get_Clock)(FW_TIM_Type *dev);
    
    void (*Set_Frequency)(FW_TIM_Type *dev, u32 freq);
    u32  (*Get_Frequency)(FW_TIM_Type *dev);
    
    void (*Timing_Init)(FW_TIM_Type *dev);
    void (*PWM_Init)(FW_TIM_Type *dev, PWM_Config_Type *config);
    void (*Encoder_Init)(FW_TIM_Type *dev, QEncoder_Config_Type *config);

    void (*Set_Counter)(FW_TIM_Type *dev, u32 value);
    u32  (*Get_Counter)(FW_TIM_Type *dev);
    
    void (*Set_DC)(FW_TIM_Type *dev, u8 ch, u16 dc);
    u16  (*Get_DC)(FW_TIM_Type *dev, u8 ch);
    
    void (*CTL)(FW_TIM_Type *dev, u8 state);
    void (*PWM_CTL)(FW_TIM_Type *dev, u8 ch, u8 state);
    void (*IT_CTL)(FW_TIM_Type *dev, u8 state);
}FW_TIM_Driver_Type;


void FW_TIM_SetPort(FW_TIM_Type *dev, void *timer);
void *FW_TIM_GetPort(FW_TIM_Type *dev);

u32  FW_TIM_GetUnit(FW_TIM_Type *dev);

void FW_TIM_SetFrequency(FW_TIM_Type *dev, u32 freq);
u32  FW_TIM_GetFrequency(FW_TIM_Type *dev);

u32  FW_TIM_GetClock(FW_TIM_Type *dev);

void FW_TIM_Init(FW_TIM_Type *dev);
void FW_TIM_CTL(FW_TIM_Type *dev, u8 state);
void FW_TIM_ITCTL(FW_TIM_Type *dev, u8 state);

void FW_TIM_BindCB(FW_TIM_Type *dev, void (*cb)(void *), void *pdata);




#ifdef __cplusplus
}
#endif

#endif

