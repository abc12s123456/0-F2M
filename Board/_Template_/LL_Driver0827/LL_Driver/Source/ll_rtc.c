#include "fw_rtc.h"
//#include "rtc_alarm.h"

#include "ll_include.h"


void RTC_IRQHandler(void)
{
    if(rtc_flag_get(RTC_FLAG_SECOND) == SET)
    {
        FW_RTC_TickTock();
        rtc_flag_clear(RTC_FLAG_SECOND);
        rtc_lwoff_wait();
    }
}


/* IRTC闹钟中断服务函数 */
void RTC_Alarm_IRQHandler(void)
{
//    FW_RTC_Type *rtc = FW_RTC_GetDevice();
    
    if(rtc_flag_get(RTC_FLAG_ALARM) == SET)
    {
//        RTC_Alarm_ISR(IRTC_HW_Alarm);
        rtc_flag_clear(RTC_FLAG_ALARM);
        exti_flag_clear(EXTI_17);
        rtc_lwoff_wait();
    }
}


__INLINE_STATIC_ void IRTC_Init(FW_RTC_Type *dev)
{
    rcu_periph_clock_enable(RCU_BKPI);            /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_PMU);
    
    pmu_backup_write_enable();                    /* allow access to BKP domain */

    rcu_osci_on(RCU_LXTAL);                       /* enable LXTAL 使用外部低速晶振作为RTC时钟源*/
    rcu_osci_stab_wait(RCU_LXTAL);                /* wait till LXTAL is ready */
    
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);       /* select RCU_LXTAL as RTC clock source */
    rcu_periph_clock_enable(RCU_RTC);             /* enable RTC Clock */
    
    rtc_register_sync_wait();                     /* wait for RTC registers synchronization */
    rtc_lwoff_wait();                             /* wait until last write operation on RTC registers has finished */
    
    rtc_interrupt_enable(RTC_INT_SECOND);         /* enable the RTC second interrupt*/
    rtc_lwoff_wait();                             /* wait until last write operation on RTC registers has finished */
    
    rtc_prescaler_set(32767);                     /* set RTC prescaler: set RTC period to 1s */
    rtc_lwoff_wait();                             /* wait until last write operation on RTC registers has finished */

    nvic_irq_enable(RTC_IRQn, 3, 3);              //RTC秒中断配置
}

__INLINE_STATIC_ void IRTC_Set_Counter(FW_RTC_Type *dev, u32 value)
{
    rtc_lwoff_wait();
    rtc_counter_set(value);
    rtc_lwoff_wait();
}

__INLINE_STATIC_ u32  IRTC_Get_Counter(FW_RTC_Type *dev)
{
    return rtc_counter_get();
}

__INLINE_STATIC_ void IRTC_Set_Alarm(FW_RTC_Type *dev, Time_Type *time)
{
    u32 counter = Time_GetCounter(time, dev->First_Year);
    static Bool flag = True;
    
    rtc_lwoff_wait();
    rtc_alarm_config(counter);
    rtc_lwoff_wait();
    
    if(flag)
    {
        exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
        exti_interrupt_flag_clear(EXTI_17);
        nvic_irq_enable(RTC_Alarm_IRQn, 3, 3);
        flag = False;
    }
}


/* Internal RTC Driver */
__CONST_STATIC_ RTC_Driver_Type IRTC_Driver =
{
    .Init = IRTC_Init,
    
    .Set_Counter = IRTC_Set_Counter,
    .Get_Counter = IRTC_Get_Counter,
    
    .Set_Alarm = IRTC_Set_Alarm,
};
FW_DRIVER_REGIST("ll->rtc", &IRTC_Driver, IRTC);



///* IRTC Alarm Driver */
//#define IRTC_ALARM_DRV_ID         FW_DRIVER_ID(FW_Device_RTC, IRTC_ALARM_DRV_NUM)

//const static RTC_Alarm_Driver_Type Alarm_Driver = 
//{
//    .Set_Alarm = IRTC_Set_Alarm,
//};
//FW_DRIVER_REGIST(IRTC_ALARM_DRV_ID, &Alarm_Driver, Alarm_Driver);

#include "fw_debug.h"
#if MODULE_TEST && RTC_TEST


static void Callback(void *pdata)
{

}


void Test(void)
{
    FW_RTC_BindCB(Callback, NULL);
}

#endif

