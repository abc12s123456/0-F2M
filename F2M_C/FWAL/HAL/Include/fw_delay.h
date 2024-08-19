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
#ifndef _FW_DELAY_H_
#define _FW_DELAY_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_mod_conf.h"


#if defined(DELAY_MOD_EN) && DELAY_MOD_EN


#include "fw_device.h"


typedef enum
{
    FW_Delay_Cmd_GetClock,        /* 获取滴答定时器时钟频率 */
    FW_Delay_Cmd_GetReload,       /* 获取滴答定时器重载值 */
    FW_Delay_Cmd_GetCounter,      /* 获取滴答定时器计数值 */
    FW_Delay_Cmd_GetTickScale,    /* 获取滴答定时器节拍时长, 单位us*/
}FW_Delay_Cmd_Enum;


typedef struct
{
    void (*Init)(void);
    
    u32  (*Get_Clock)(void);
    
    u32  (*Get_Reload)(void);
    u32  (*Get_Counter)(void);
    
    u32  (*Get_TickTime)(void);
}FW_Delay_Driver_Type;


typedef struct
{
    u32 Tick;
    u32 Counter;
}FW_Delay_Time_Type, fdtt;


/**
 * @API
 */
void FW_Delay_Init(void);

void FW_Delay_BindOS(void (*os_delay)(u32));

void FW_Delay_Ns(u32 nns);
void FW_Delay_Us(u32 nus);
void FW_Delay_Ms(u32 nms);

void FW_Delay_BindCB(void (*cb)(void *), void *pdata);

void FW_Delay_TickTock(void);

u32  FW_Delay_GetTick(void);
u32  FW_Delay_GetTickTime(void);
u32  FW_Delay_GetPWROnTime(void);

u32  FW_Delay_GetMsStart(void);
u32  FW_Delay_GetMsDuration(u32 start_time);

fdtt FW_Delay_GetUsStart(void);
u32  FW_Delay_GetUsDuration(fdtt start_time);

void FW_Delay_RoughUs(u32 nus);
void FW_Delay_RoughMs(u32 nms);


#endif /* defined(DELAY_MOD_EN) && DELAY_MOD_EN */


#ifdef __cplusplus
}
#endif

#endif

