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
#include "fw_wdg.h"


#if defined(WDG_MOD_EN) && WDG_MOD_EN


#include "fw_debug.h"
#include "fw_print.h"


#define WDG_MONITOR_TICK_MAX           100


/* 内部独立看门狗 */
FW_WDG_Type IWDG;
static void IWDG_Driver_Probe(void *dev)
{
    WDG_Driver_Type **drv = (WDG_Driver_Type **)(&IWDG.Device.Driver);
    *drv = FW_Driver_Find("ll->iwdg");
    if(*drv == NULL)
    {
        /* internal iwdg driver is not instanced */
        LOG_D("internal iwdg driver is not instanced\r\n");
//        while(1);
    }
}
FW_DEVICE_STATIC_REGIST("iwdg", &IWDG, IWDG_Driver_Probe, IWDG);

/* 内部窗口看门狗 */
static FW_WDG_Type WWDG;
static void WWDG_Driver_Probe(void *dev)
{
    WDG_Driver_Type **drv = (WDG_Driver_Type **)(&WWDG.Device.Driver);
    *drv = FW_Driver_Find("ll->wwdg");
    if(*drv == NULL)
    {
        /* internal iwdg driver is not instanced */
        LOG_D("internal wwdg driver is not instanced\r\n");
//        while(1);
    }
}
FW_DEVICE_STATIC_REGIST("wwdg", &WWDG, WWDG_Driver_Probe, WWDG);


void WDG_Init(FW_WDG_Type *dev, u32 wdg_time)
{
    WDG_Driver_Type *drv= FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Init);
    
    dev->WDG_Time = wdg_time;
    dev->Monitor_Tick = 0;
    dev->Monitor_Flag = False;
    
    drv->Init(dev, wdg_time);
}

/* 喂狗 */
void WDG_FeedDog(FW_WDG_Type *dev)
{
    WDG_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->FeedDog);
    
    drv->FeedDog(dev);
    
    /* 重新配置看门时间后，强制咬狗 */
    if(dev->Bit_Flag)
    {
        WDG_Init(dev, dev->WDG_Time);
        while(1);
    }
}

/* 强制咬狗 */
void WDG_BitDog(FW_WDG_Type *dev)
{
    WDG_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Get_MinTime);

    dev->WDG_Time = drv->Get_MinTime(dev);
    dev->Bit_Flag = True;
    
    WDG_Init(dev, dev->WDG_Time);
    while(1);
}

void WDG_BindCB(FW_WDG_Type *dev, void (*cb)(void *), void *pdata)
{
    FW_ASSERT(dev);
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
}

/* 周期性喂狗，在周期性中断中调用，period:中断周期，单位：ms */
void WDG_FeedDogISR(FW_WDG_Type *dev, u32 period)
{
    WDG_Driver_Type *drv = FW_Device_GetDriver(dev);
    u32 time = dev->WDG_Time >> 1;
    
    /* 中途喂一次狗 */
    if(dev->Monitor_Tick++ * period >= time)
    {
        if(dev->Monitor_Flag != True)
        {
            WDG_BitDog(dev);
            while(1);
        }
        
        dev->Monitor_Tick = 0;
        dev->Monitor_Flag = False;
        drv->FeedDog(dev);
    }
}

/* 在被监测的任务中周期性调用，任务的执行周期需远小于喂狗周期 */
void WDG_TaskMonitor(FW_WDG_Type *dev)
{
    dev->Monitor_Flag = True;
}


#endif  /* defined(WDG_MOD_EN) && WDG_MOD_EN */

