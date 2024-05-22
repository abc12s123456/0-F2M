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
 
#ifndef _FW_RTC_H_
#define _FW_RTC_H_

#ifdef __cplusplus
extern "{"
#endif


#include "fw_device.h"
#include "time.h"


#define RTC_DRV_NAT_NUM         0      //内部RTC
#define RTC_DRV_EXT_NUM         1      //外部RTC


typedef struct
{
    FW_Device_Type Device;
    
    void (*IH_CB)(void *);
    void *IH_Pdata;
    
    u32  Counter;
    u32  Tick;

    u32  First_Year : 16;
    
    u32  : 16;
    
    Time_Type Time;
    
    /* RTC闹钟管理链表 */
    __RO_ FW_SList_Type *CList;        //当前已响应的硬闹钟
    
    __RO_ FW_SList_Type *HList;        //硬闹钟
    __RO_ FW_SList_Type *SList;        //软闹钟
}FW_RTC_Type;


typedef struct
{
    void (*DeInit)(FW_RTC_Type *dev);
    void (*Init)(FW_RTC_Type *dev);
    
    u32  (*Get_Counter)(FW_RTC_Type *dev);
    void (*Set_Counter)(FW_RTC_Type *dev, u32 value);
    
    Bool (*Get_Time)(FW_RTC_Type *dev, Time_Type *time);
    void (*Set_Time)(FW_RTC_Type *dev, const Time_Type *time);
    
    /* RTC源闹钟设置 */
    void (*Set_Alarm)(FW_RTC_Type *dev, Time_Type *time);
}RTC_Driver_Type;


void RTC_Init(FW_RTC_Type *dev);

Bool RTC_Get_Time(FW_RTC_Type *dev, Time_Type *time);
void RTC_Set_Time(FW_RTC_Type *dev, Time_Type *time);

u32  RTC_Get_Counter(FW_RTC_Type *dev);
void RTC_Set_Counter(FW_RTC_Type *dev, u16 first_year, u32 counter);

u32  RTC_Get_Tick(FW_RTC_Type *dev);

void RTC_TickTock(FW_RTC_Type *dev);

void RTC_Bind_CB(FW_RTC_Type *dev, void (*cb)(void *), void *pdata);

void RTC_Set_Wakeup(FW_RTC_Type *dev, u32 slp_time);


/**
 * @FW_RTC
 */
extern FW_RTC_Type IRTC;

#define FW_RTC_GetDevice()  (&IRTC)

#if defined(RTC_RELOAD_EN) && (RTC_RELOAD_EN)

#define FW_RTC_Init(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 0)  RTC_Init(&IRTC);\
    else if(n == 1)  RTC_Init((FW_RTC_Type *)VA0(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_RTC_GetTime(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? RTC_Get_Time(&IRTC, (Time_Type *)VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? RTC_Get_Time((FW_RTC_Type *)VA0(__VA_ARGS__), (Time_Type *)VA1(__VA_ARGS__)) :\
(Bool)VA_ARGS_NUM_ERR())

#define FW_RTC_SetTime(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  RTC_Set_Time(&IRTC, (Time_Type *)VA0(__VA_ARGS__));\
    else if(n == 2)  RTC_Set_Time((FW_RTC_Type *)VA0(__VA_ARGS__), (Time_Type *)VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_RTC_GetCounter(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? RTC_Get_Counter(&IRTC) :\
(VA_NUM(__VA_ARGS__) == 1) ? RTC_Get_Counter((FW_RTC_Type *)VA0(__VA_ARGS__)) :\
(u32)VA_ARGS_NUM_ERR())

#define FW_RTC_SetCounter(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 2)  RTC_Set_Counter(&IRTC, (u16)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__));\
    else if(n == 3)  RTC_Set_Counter((FW_RTC_Type *)VA0(__VA_ARGS__), (u16)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_RTC_GetTick(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? RTC_Get_Tick(&IRTC) :\
(VA_NUM(__VA_ARGS__) == 1) ? RTC_Get_Tick((FW_RTC_Type *)VA0(__VA_ARGS__)) :\
(u32)VA_ARGS_NUM_ERR())

#define FW_RTC_TickTock(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 0)  RTC_TickTock(&IRTC);\
    else if(n == 1)  RTC_TickTock((FW_RTC_Type *)VA0(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_RTC_BindCB(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 2)  RTC_Bind_CB(&IRTC, (void *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__));\
    else if(n == 3)  RTC_Bind_CB((FW_RTC_Type *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_RTC_SetWakeup(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  RTC_Set_Wakeup(&IRTC, (void *)VA0(__VA_ARGS__));\
    else if(n == 2)  RTC_Set_Wakeup((FW_RTC_Type *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#else

__INLINE_STATIC_ void FW_RTC_Init(void)
{
    RTC_Init(&IRTC);
}

__INLINE_STATIC_ Bool FW_RTC_GetTime(Time_Type *time)
{
    return RTC_Get_Time(&IRTC, time);
}

__INLINE_STATIC_ void FW_RTC_SetTime(Time_Type *time)
{
    RTC_Set_Time(&IRTC, time);
}

__INLINE_STATIC_ u32  FW_RTC_GetCounter(void)
{
    return RTC_Get_Counter(&IRTC);
}

__INLINE_STATIC_ void FW_RTC_SetCounter(u16 first_year, u32 counter)
{
    RTC_Set_Counter(&IRTC, first_year, counter);
}

__INLINE_STATIC_ u32  FW_RTC_GetTick(void)
{
    return RTC_Get_Tick(&IRTC);
}

__INLINE_STATIC_ void FW_RTC_TickTock(void)
{
    RTC_Tick_Tock(&IRTC);
}

__INLINE_STATIC_ void FW_RTC_BindCB(void (*cb)(void *), void *pdata)
{
    RTC_Bind_CB(&IRTC, cb, pdata);
}

__INLINE_STATIC_ void FW_RTC_Wakeup(u32 slp_time)
{
    RTC_Wakeup(&IRTC, slp_time);
}

#endif  /* #if defined(RTC_RELOAD_EN) && (RTC_RELOAD_EN) */


#ifdef __cplusplus
}
#endif

#endif

