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
#include "fw_delay.h"
#include "fw_system.h"
#include "fw_debug.h"
#include "fw_print.h"


#define NS_MULT     1000


static FW_Delay_Driver_Type *Driver;

static volatile u32 Tick = 0;
static volatile u32 Factor_Us;
static volatile u32 Factor_Ns;
static volatile u32 Tick_Time = 0;               //每个Tick的时间，单位：us

static          s8  Duration_Us_Bias = 0;        //


#if (RTOS_EN == ON)
__WEAK_ void FW_OS_TickTock(void){}
#endif
//__WEAK_ __INLINE_ Bool FW_OS_IsStart(void){return False;}
//__WEAK_ __INLINE_ void FW_OS_Delay(u32 nms){}
//__WEAK_ __INLINE_ u32  FW_OS_GetTickTime(void){return 0;}
extern Bool FW_OS_IsStart(void);
extern void FW_OS_Delay(u32 nms);
extern u32  FW_OS_GetTickTime(void);


/* Delay模块回调函数，可以在应用时由用户配置 */
#if (DELAY_CB_EN == ON)
static void (*PF_IH_CB)(void *);
static void *IH_Pdata;
__INLINE_STATIC_ void IH_CB_Null(void *pdata){}
#endif


static void (*PF_Delay_Ms)(u32 nms);
__INLINE_STATIC_ void FW_Delay_DefMs(u32 nms);


/**
@功能：延时初始化
@参数：无
@返回：无
@说明：1、由平台提供硬件初始化
       2、当使用OS时，应在OS启动后再开始初始化Delay，这样延时才会使用OS的系统延时
*/
void FW_Delay_Init(void)
{
    fdtt start;
    u32 duration;
    
    Driver = FW_Driver_Find("ll->delay");
    if(Driver == NULL)
    {
        /* delay driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->Init);
    FW_ASSERT(Driver->Get_Clock);
    FW_ASSERT(Driver->Get_TickTime);
    
    /* 获取us计时因子 */
    Factor_Us = Driver->Get_Clock() / 1000000;
    
    /* 获取ns计时因子 */
    Factor_Ns = 1000 * NS_MULT / (Driver->Get_Clock() / 1000000);
    
    /* OS开启后，使用系统延时 */
    if(FW_OS_IsStart() == True)
    {
        PF_Delay_Ms = FW_OS_Delay;
        Tick_Time = FW_OS_GetTickTime();
    }
    else
    {
        PF_Delay_Ms = FW_Delay_DefMs;
        Tick_Time = Driver->Get_TickTime();
    }
    
    
#if (DELAY_CB_EN == ON)
    PF_IH_CB = IH_CB_Null;
#endif
    
    /* 初始化框架的时钟节拍 */
	Driver->Init();
    
    /* 计算FW_Delay_GetUsDuration函数获取us级别时长时的偏差 */
    start = FW_Delay_GetUsStart();
    FW_Delay_Us(1);
    duration = FW_Delay_GetUsDuration(start);
    Duration_Us_Bias = duration - 1;
}
/**/

/**
@功能：延时绑定OS
@参数：无
@返回：无
@说明：1、供HAL_OS_Start调用。绑定后，由OS提供ms延时
*/
void FW_Delay_BindOS(void (*os_delay)(u32))
{
    FW_Enter_Critical();
    PF_Delay_Ms = os_delay;
    FW_Exit_Critical();
}
/**/

/**
@功能：ns延时
@参数：nns，延时的纳秒数
@返回：无
@说明：1、ns级的延时一般只能由使用平台提供，例如CM3/CM4/CM7系列的systick计数器或
          者根据时钟通过循环进行延时
       2、该函数适用于延时基准不超过1GHz的情况
       3、该函数有一个最小延时时间，当输入的参数小于该值时，延时时间自动调整为最
          值。同时入参也不能太大，保证(nns * NS_MULT) <= U32_MAX
*/
void FW_Delay_Ns(u32 nns)
{
    u32 t_old, t_now, t_cnt;
    u32 tick;
    u32 reload;
    
    FW_ASSERT(Driver->Get_Reload);
    FW_ASSERT(Driver->Get_Counter);
    
    nns *= NS_MULT;
    if(nns < Factor_Ns)
    {
        nns = Factor_Ns;
    }
    
    t_cnt = 0;
    tick = nns / Factor_Ns;
    reload = Driver->Get_Reload();
    
    t_old = Driver->Get_Counter();
    while(1)
    {
        t_now = Driver->Get_Counter();
        if(t_now != t_old)
        {
            if(t_now < t_old)  t_cnt += t_old - t_now;
            else  t_cnt += reload - t_now + t_old;
            
            t_old = t_now;
            
            if(t_cnt >= tick)  break;
        }
    }
}
/**/

/**
@功能：us延时
@参数：nus, 延时的us数
@返回：无
@说明：1、us级的延时一般只能由使用平台提供，例如CM3/CM4/CM7系列的systick计数器或
          者根据时钟通过循环进行延时
*/
void FW_Delay_Us(u32 nus)
{
    u32 t_old, t_now, t_cnt;                                                    //节拍计数
    u32 tick;                                                                   //延时需要的节拍数
    u32 reload;                                                                 //systick重载值

    FW_ASSERT(Driver->Get_Reload);
    FW_ASSERT(Driver->Get_Counter);
    
    t_cnt = 0;
    tick = nus * Factor_Us;
    reload = Driver->Get_Reload();
    
    t_old = Driver->Get_Counter();                                              //刚进入时的计数值  
    while(1)
    {
        t_now = Driver->Get_Counter();                                          //查询当前计数
        if(t_now != t_old)                                                      //判断延时是否已到
        {
            if(t_now < t_old)  t_cnt += t_old - t_now;                          //systick定时器未溢出时已经过的节拍数
            else  t_cnt += reload - t_now + t_old;                              //定时器溢出(只考虑一轮溢出的情况)

            t_old = t_now;

            if(t_cnt >= tick)  break;                                           //延时到则退出，否则执行while循环
        }
    }
}
/**/

/**
@功能：ms延时
@参数：nms, 延时的ms数
@返回：无
@说明：1、最小延时单位必须为系统时钟中断时间的整数倍，例如：当时钟1ms中断一次时，
          延时时间可以是范围内的任意整数，而当时钟5ms中断一次时，延时时间则为5ms、
          10ms、15ms......方便起见，时钟中断时间一般设置为1次/ms
*/
void FW_Delay_Ms(u32 nms)
{
    PF_Delay_Ms(nms);
}
/**/

void FW_Delay_BindCB(void (*cb)(void *), void *pdata)
{
#if (DELAY_CB_EN == ON)
    FW_Enter_Critical();
    PF_IH_CB = cb;
    IH_Pdata = pdata;
    FW_Exit_Critical();
#endif
}

/**
@功能：运行时钟节拍(时钟的滴答声)
@参数：无
@返回：无
@说明：1、周期调用该函数，为Delay模块提供计时节拍
       2、Tick仅在该函数中进行修改，其它函数只读取，所以无需进行临界保护
*/
void FW_Delay_TickTock(void)
{
    Tick++;
    
#if (DELAY_CB_EN == ON)
    PF_IH_CB(IH_Pdata);
#endif
    
#if (RTOS_EN == ON && (MODULE_TEST == OFF || (MODULE_TEST == ON && RTOS_TEST == ON)))
    FW_OS_TickTock();
#endif
}
/**/

/**
@功能：获取当前计数节拍的数值
@参数：无
@返回：当前系统时钟节拍（Tick）的值
@说明：无
*/
u32  FW_Delay_GetTick(void)
{
    return Tick;
}
/**/

/**
@功能：获取系统节拍时间(Get time per tick)
@参数：无
@返回：一个系统节拍的时间,单位us
@说明：无
*/
u32  FW_Delay_GetTickTime(void)
{
    FW_ASSERT(Driver->Get_TickTime);
    
    Tick_Time = Driver->Get_TickTime();
    
    return Tick_Time;
}
/**/

/**
@功能：获取系统开机以后到检测时的累计时间(Get time boot up)
@参数：无
@返回：us数
@说明：1、不考虑系统长时间运行时的计数溢出情况
*/
u32  FW_Delay_GetPWROnTime(void)
{
    return (u32)(Tick_Time * Tick);
}
/**/


u32  FW_Delay_GetMsStart(void)
{
    return Tick;
}
/**/

u32  FW_Delay_GetMsDuration(u32 start_time)
{
    u32 tick, tms;
    
    tick = Tick;
    
    if(tick >= start_time)
    {
        tms = tick - start_time;
    }
    else
    {
        tms = tick + 0xFFFFFFFF - start_time;
    }
    
    return (tms * Tick_Time / 1000);
}
/**/

/**
@功能：获取持续时间的起始时刻
@参数：无
@返回：起始时刻，单位：计数值
@说明：以delay使用的定时器计数值为基准;
*/
fdtt FW_Delay_GetUsStart(void)
{
    fdtt time;
    
    FW_ASSERT(Driver->Get_Counter);
    
    time.Tick = Tick;
    time.Counter = Driver->Get_Counter();
    
    return time;
}
/**/

/**
@功能：获取指定起始时到检测时的持续时间
@参数：start_time, 起始时间
@返回：持续时间，单位：us
@说明：1、以delay使用的定时器计数值为基准;
       2、最大能表示的时间(0xFFFFFFFF / 1000000)s，约71.58分钟
*/
u32  FW_Delay_GetUsDuration(fdtt start_time)
{
    u32 tms, tcnt;
    u32 tmp, start_tmp;
    u32 reload;
    
    /* 获取当前时刻的Tick值 */
    tmp = Tick;
    start_tmp = start_time.Tick;
    
    if(tmp >= start_tmp)
    {
        tms = tmp - start_tmp;
    }
    else
    {
        tms = tmp + 0xFFFFFFFF - start_tmp;
    }
    
    /* 获取时钟的计数值 */
    tmp = Driver->Get_Counter();
    start_tmp = start_time.Counter;
    reload = Driver->Get_Reload();
    
    /* 计数器统一以递减方式计算，由底层进行适配 */
    if(tmp <= start_tmp)
    {
        tcnt = start_tmp - tmp;
    }
    else
    {
        tms -= 1;
        tcnt = (reload + 1) - (tmp - start_tmp);
    }
    
    /* 将总时间转换为us */
    tms *= Tick_Time;
    tcnt /= Factor_Us;
    
    if(tms == 0)  return (tcnt - Duration_Us_Bias);
    return (tms + tcnt);
}
/**/

/**
@功能：粗略的us延时
@参数：nus, 延时时间
@返回：无
@说明：1、根据系统的主时钟，通过循环进行粗略延时。适用于系统无法提供节拍时或不需
          要较精确延时的应用
       2、需要底层提供系统运行的主时钟频率
*/
void FW_Delay_RoughUs(u32 nus)
{
    u32 sysclk = FW_System_GetSysclk();
    u32 cnt = sysclk / 4000000 * nus;            //延时需要执行的指令数量
    u32 i;
    
    for(i = 0; i < cnt; i++);
}
/**/

/**
@功能：粗略的ms延时
@参数：nms, 延时时间
@返回：无
@说明：1、根据系统的主时钟，通过循环进行粗略延时。适用于系统无法提供节拍时或不需
          要较精确延时的应用
       2、需要底层提供系统运行的主时钟频率
*/
void FW_Delay_RoughMs(u32 nms)
{
    u32 sysclk = FW_System_GetSysclk();
    u32 cnt = sysclk / 4000 * nms;               //延时需要执行的指令数量
    u32 i;
    
    for(i = 0; i < cnt; i++);
}
/**/


__INLINE_STATIC_ void FW_Delay_DefMs(u32 nms)
{
    if(nms == 0)  return;
    
    u32 tick0 = 0;
    nms = nms * 1000 / Driver->Get_TickTime();
    
    tick0 = Tick;                                //读取当前的Tick值
  
    while(1)                                     //循环，当延时到方可跳出
    {
        if(tick0 <= Tick)					     //Tick还在一个计数周期内
        {
            if(nms <= Tick - tick0)  
            {
                break;                           //延时到，跳出while循环
            }
        }
        else							         //Tick已经在下一个计数周期
        {
            if(nms <= 0xFFFFFFFF - tick0 + Tick)  
            {
                break;                           //延时到，跳出while循环
            }
        }
    }
}

