#include "ll_include.h"
#include "ll_system.h"


#include "fw_delay.h"


#if 0
extern void lv_tick_inc(u32 period);


__LI_ void SysTick_Handler(void)
{
    FW_Delay_TickTock();
    lv_tick_inc(1);
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


void HAL_Delay(uint32_t Delay)
{
    FW_Delay_Ms(Delay);
}




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

#else

extern void lv_tick_inc(u32 period);


__LI_ void TIM2_IRQHandler(void)
{
    LL_TIM_ClearFlag_UPDATE(TIM2);
    FW_Delay_TickTock();
    lv_tick_inc(1);
}

static u32 Reload;

__INLINE_STATIC_ void Delay_Init(void)
{
    u32 clk;
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_SetPrescaler(TIM2, __LL_TIM_CALC_PSC(84000000, 1000000));
    
    clk = 84000000;
    
    Reload = __LL_TIM_CALC_ARR(clk, LL_TIM_GetPrescaler(TIM2), 1000);
    LL_TIM_SetAutoReload(TIM2, Reload);
    
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_DOWN);
    
    LL_TIM_EnableIT_UPDATE(TIM2);
    
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);
    
    /* Enable counter */
    LL_TIM_EnableCounter(TIM2);

    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

__INLINE_STATIC_ u32 Delay_Get_Clock(void)
{
    return LL_Clocks.AHBClk / 84;
}

__INLINE_STATIC_ u32 Delay_Get_Reload(void)
{
//    return SysTick->LOAD;
    return TIM2->ARR;
}

__INLINE_STATIC_ u32 Delay_Get_Counter(void)
{
//    return SysTick->VAL;
    return TIM2->CNT;
//    return (Reload - TIM2->CNT);
}

__INLINE_STATIC_ u32 Delay_Get_TickTime(void)
{
    /* 根据Systick设置的中断时间确定，1000表示一个系统节拍的时间为1000us */
    return 1000;
}


void HAL_Delay(uint32_t Delay)
{
    FW_Delay_Ms(Delay);
}




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


#endif





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

