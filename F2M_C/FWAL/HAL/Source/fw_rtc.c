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
 
#include "fw_rtc.h"

#include "fw_debug.h"
#include "fw_print.h"


FW_RTC_Type IRTC;

static void IRTC_Driver_Probe(void *dev)
{
    RTC_Driver_Type **drv = (RTC_Driver_Type **)(&IRTC.Device.Driver);
    *drv = FW_Driver_Find("ll->rtc");
    if(*drv == NULL)
    {
        /* internal rtc driver is not instanced */
        LOG_D("internal rtc driver is not instanced\r\n");
    }
}
FW_DEVICE_STATIC_REGIST("irtc", &IRTC, IRTC_Driver_Probe, IRTC);

extern void FW_SDI(void);
extern void FW_Lock(void);
extern void FW_Unlock(void);


#define RTC_SDI         FW_SDI
#define RTC_Lock        FW_Lock
#define RTC_Unlock      FW_Unlock


void RTC_Init(FW_RTC_Type *dev)
{
    RTC_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool flag = False;
    
    if(dev->First_Year < 1970)
    {
        dev->First_Year = 1970;
    }
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Init);
    
    drv->Init(dev);
    
    /* 获取当前时间 */
    RTC_Get_Time(dev, &dev->Time);
    
    /* 计时时间的年份默认不早于起始元年 */
    if(dev->Time.Year < dev->First_Year)
    {
        dev->Time.Year = dev->First_Year;
        flag = True;
    }
    
    /* 若设置时间值不合法，则恢复默认值 */
    if(Time_StructAssert(&dev->Time) != True)
    {
        dev->Time.Month = 1;
        dev->Time.Day = 1;
        dev->Time.Hour = 0;
        dev->Time.Minute = 0;
        dev->Time.Second = 0;
        flag = True;
    }
    
    /* 时间值有修改时，重新设置时间 */
    if(flag)
    {
        RTC_Set_Time(dev, &dev->Time);
    }
}

Bool RTC_Get_Time(FW_RTC_Type *dev, Time_Type *time)
{
    RTC_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv->Get_Counter)
    {
        dev->Counter = drv->Get_Counter(dev);
        Time_GetTime(&dev->Time, dev->First_Year, dev->Counter);
    }
    else if(drv->Get_Time)
    {
        drv->Get_Time(dev, &dev->Time);
        dev->Counter = Time_GetCounter(&dev->Time, dev->First_Year);
    }
    else
    {
        LOG_D("rtc%d lack of underlying implementation of \"Get_Counter\" or \"Get_Time\" function", pdev->Parent.ID);
        return False;
    }
    
    if(time != &dev->Time)
    {
        /* 输出至time */
        memcpy(time, &dev->Time, sizeof(Time_Type));
    }
    
    return True;
}

void RTC_Set_Time(FW_RTC_Type *dev, Time_Type *time)
{
    RTC_Driver_Type *drv;
    
    if(Time_StructAssert(time) != True)  return;
    
    RTC_Lock();
    
    /* 同步更新计时时间 */
    memcpy(&dev->Time, time, sizeof(Time_Type));
    
    dev->Counter = Time_GetCounter(&dev->Time, dev->First_Year);
    
    drv = FW_Device_GetDriver(dev);
    
    if(drv->Set_Counter)  drv->Set_Counter(dev, dev->Counter);
    else if(drv->Set_Time)  drv->Set_Time(dev, &dev->Time);
    else
    {
        LOG_D("rtc%d lack of underlying implementation of \"Set_Counter\" or \"Set_Time\" ");
    }
    
    RTC_Unlock();
}

u32  RTC_Get_Counter(FW_RTC_Type *dev)
{
    RTC_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv->Get_Counter)
    {
        dev->Counter = drv->Get_Counter(dev);
        Time_GetTime(&dev->Time, dev->First_Year, dev->Counter);
    }
    else if(drv->Get_Time)
    {
        drv->Get_Time(dev, &dev->Time);
        dev->Counter = Time_GetCounter(&dev->Time, dev->First_Year);
    }
    else
    {
        LOG_D("rtc%d lack of underlying implementation of \"Get_Counter\" or \"Get_Time\" function", pdev->Parent.ID);
        dev->Counter = 0;
    }
    
    return dev->Counter;
}

void RTC_Set_Counter(FW_RTC_Type *dev, u16 first_year, u32 counter)
{
    Time_Type time;
    Time_GetTime(&time, first_year, counter);
    RTC_Set_Time(dev, &time);
}

u32  RTC_Get_Tick(FW_RTC_Type *dev)
{
    return dev->Tick;
}

void RTC_TickTock(FW_RTC_Type *dev)
{
    dev->Tick++;
    if(dev->IH_CB)  dev->IH_CB(dev->IH_Pdata);
}

void RTC_Bind_CB(FW_RTC_Type *dev, void (*cb)(void *), void *pdata)
{
    RTC_Lock();
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
    RTC_Unlock();
}

/**
@功能: RTC唤醒
@参数: dev, RTC设备
       slp_time, 休眠时间，单位s
@返回: 无
@备注: 1、一般情况下，RTC可以唤醒待机休眠(无法唤醒其它休眠模式)；
       2、RTC通过闹钟进行休眠唤醒
*/
void RTC_Set_Wakeup(FW_RTC_Type *dev, u32 slp_time)
{
    RTC_Driver_Type *drv = FW_Device_GetDriver(dev);
    Time_Type time;
    
    /* 防止与硬件闹钟冲突，设置待机唤醒时，关闭中断 */
    RTC_SDI();
    
    slp_time += RTC_Get_Counter(dev);
    Time_GetTime(&time, dev->First_Year, slp_time);
    
    drv->Set_Alarm(dev, &time);
}

