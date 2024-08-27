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
 
#include "stiming.h"

#include "fw_system.h"
#include "fw_delay.h"


#define STiming_Lock         FW_Enter_Critical
#define STiming_Unlock       FW_Exit_Critical


/**
@功能：软件定时器缺省配置
@参数：Type, 定时器对象
@返回：无
@说明：@使用HAL_Delay_GetTick作为定时节拍
*/
void STiming_DeInit(STiming_Type *t)
{
    t->Get_Tick = FW_Delay_GetTick;   
    
    t->CB = NULL;
    t->CB = NULL;
    
    t->Tick0 = 0;
    
    t->Mode = STiming_Mode_Period;             //定时器默认为周期模式
    t->Get_Flag = False;
    t->Switch_Flag = True;                     //定时器默认打开
    t->Ignore_Flag = False;                    //该标识置位时，定时器总是返回True
    t->Done_Flag = True;                       //定时器未执行时，认为定时完成
}
/**/

/**
@功能：软件定时器初始化
@参数：Type, 定时器对象
       Get_Tick, 定时节拍回调函数指针
@返回：无
@说明：@需要用户提供一个定时节拍才能开始工作
@记录：1，2020-10-15， 优化，当Get_Tick=NULL时，使用HAL_Delay_GetTick作为默认输入
*/
void STiming_Init(STiming_Type *t, u32 (*get_tick)(void))
{
    if(get_tick)  t->Get_Tick = get_tick;
    else  t->Get_Tick = FW_Delay_GetTick;
    
    t->Tick0 = 0;
    
    t->Mode = STiming_Mode_Period;             //定时器默认为周期模式
    t->Get_Flag = False;
    t->Switch_Flag = True;                     //定时器初始化完成后，默认打开
    t->Ignore_Flag = False;                    //该标识置位时，定时器总是返回True
    t->Done_Flag = True;                       //定时器未执行时，认为定时完成

    t->CB = NULL;
}
/**/

void STiming_SetCB(STiming_Type *t, void (*cb)(void *), void *pdata)
{
    STiming_Lock();
    t->CB = cb;
    t->Pdata = pdata;
    STiming_Unlock();
}

/**
@功能：软件定时器运行
@参数：Type, 定时器对象
       timing_time, 定时时间，单位以t->Get_Tick为准
@返回：True, 定时时间到
       False, 定时时间未到
@说明：@定时器到达后，会重新开始新一轮定时
*/
Bool STiming_Start(STiming_Type *t, u32 timing_time)
{
    if(timing_time == 0)  goto Timing_Done;;     //定时时间为0，始终返回True
    if(t->Switch_Flag == False)  return False;   //定时关闭，不会到达
    if(t->Ignore_Flag == True)  goto Timing_Done;//定时忽略，始终返回True
    
    if(t->Mode == STiming_Mode_Once)
    {
        if(t->Done_Flag == True)  return False;
    }
    
    STiming_Lock();
    if(t->Get_Flag != True)
    {
        t->Tick0 = t->Get_Tick();
        t->Get_Flag = True;                      //定时起点已获取
    }
    STiming_Unlock();
    
    if(t->Tick0 <= t->Get_Tick())
    {
        if(timing_time <= t->Get_Tick() - t->Tick0)
        {
            t->Get_Flag = False;
            t->Done_Flag = True;
            goto Timing_Done;
        }
        return False;
    }
    else
    {
        if(timing_time <= 0xFFFFFFFF - t->Tick0 + t->Get_Tick())
        {
            t->Get_Flag = False;
            t->Done_Flag = True;
            goto Timing_Done;
        }
        return False;
    }
    
    Timing_Done:
    if(t->CB)  t->CB(t->Pdata);
    
    return True;
}
/**/

/**
@功能：软件定时器复位
@参数：Type, 定时器对象
@返回：无
@说明：@使用该函数后，定时器会重新开始定时
*/
void STiming_Reset(STiming_Type *t)
{
    STiming_Lock();
    t->Get_Flag = False;
    t->Switch_Flag = True;
    t->Done_Flag = False;
    t->Ignore_Flag = False;
    STiming_Unlock();
}
/**/

/**
@功能：软件定时器打开
@参数：Type, 定时器对象
@返回：无
@说明：@使用该函数后，定时器会打开
       @该函数不影响定时时间
*/
void STiming_Open(STiming_Type *t)
{
    STiming_Lock();
    t->Switch_Flag = True;
    STiming_Unlock();
}
/**/

/**
@功能：软件定时器关闭
@参数：Type, 定时器对象
@返回：无
@说明：@使用该函数后，定时器会关闭
       @该函数不影响定时时间
*/
void STiming_Close(STiming_Type *t)
{
    STiming_Lock();
    t->Switch_Flag = False;
    STiming_Unlock();
}
/**/

/**
@功能：软件定时器完成
@参数：Type, 定时器对象
@返回：无
@说明：@使用该函数后，定时器会一直处于定时到达状态，直到系统复位或调用
        Timing_Reset函数
*/
void STiming_Done(STiming_Type *t)
{
    STiming_Lock();
    t->Ignore_Flag = True;
    STiming_Unlock();
}
/**/

