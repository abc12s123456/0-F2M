#include "ll_include.h"
#include "ll_system.h"


#include "fw_delay.h"


__LI_ void SysTick_Handler(void)
{
    FW_Delay_TickTock();
}


__INLINE_STATIC_ void Delay_Init(void)
{
    SysTick_Config(LL_Clocks.AHBClk / 1000);
    
    /* Systick中断优先级不能设置过低，否则会导致其它应用工作异常(例如emwin) */
    NVIC_SetPriority(SysTick_IRQn, 0);
}

__INLINE_STATIC_ u32 Delay_Get_Clock(void)
{
    return LL_Clocks.AHBClk;
}

__INLINE_STATIC_ u32 Delay_Get_Reload(void)
{
    return SysTick->LOAD;
}

__INLINE_STATIC_ u32 Delay_Get_Counter(void)
{
    return SysTick->VAL;
}

__INLINE_STATIC_ u32 Delay_Get_TickTime(void)
{
    /* 根据Systick设置的中断时间确定，1000表示一个系统节拍的时间为1000us */
    return 1000;
}


/*需要为固件库底层提供的时钟节拍和延时函数*/
u32 HAL_GetTick(void)
{
    return FW_Delay_GetTick();
}

void HAL_Delay(u32 t)
{
    FW_Delay_Ms(t);
}
/**/




/* Delay Driver */
__CONST_STATIC_ FW_Delay_Driver_Type Driver =
{
    .Init         = Delay_Init,
    .Get_Clock    = Delay_Get_Clock,
    .Get_Reload   = Delay_Get_Reload,
    .Get_Counter  = Delay_Get_Counter,
    .Get_TickTime = Delay_Get_TickTime,
};
FW_DRIVER_REGIST("ll->delay", &Driver, Delay);





#include "fw_debug.h"
#if MODULE_TEST && DELAY_TEST


#define CNT_MAX     10

static volatile u32 Duration[CNT_MAX * 2];


void Test(void)
{
    fdtt start;
    u32 duration;
    u32 i;
    u8 cnt = 0;
    
    while(1)
    {
        start = FW_Delay_GetUsStart();
        FW_Delay_Ns(5000);
        duration = FW_Delay_GetUsDuration(start);
        Duration[cnt] = duration;
        
        FW_Delay_Ms(5);
        
        start = FW_Delay_GetUsStart();
//        FW_Delay_Ms(1000);
        FW_Delay_Us(123);
        duration = FW_Delay_GetUsDuration(start);
        Duration[cnt + CNT_MAX] = duration;
        
        if(++cnt >= CNT_MAX)  cnt = 0;
        
        FW_Delay_Ms(5);
    }
}


#endif

