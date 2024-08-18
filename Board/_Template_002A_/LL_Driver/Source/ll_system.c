#include "ll_include.h"
#include "ll_system.h"

#include "fw_system.h"


LL_Clocks_Type LL_Clocks;


//当外部时钟出现故障时，会触发NMI中断；可以在这里进行时钟切换
__LI_ void NMI_Handler(void)
{
#if HSE_EN
    if(rcu_interrupt_flag_get(RCU_INT_FLAG_HXTALSTB) == SET)
    {
        //使能内部时钟
        rcu_osci_on(RCU_IRC8M);
        while(rcu_flag_get(RCU_FLAG_IRC8MSTB) != SET);
        
        //使用内部时钟作为PLL输入
        rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2, RCU_PLL_MUL30);  //PLL_CLK = 8MHz / 2 * 30 = 120MHz
        rcu_osci_on(RCU_PLL_CK);
        while(rcu_flag_get(RCU_FLAG_PLLSTB) != SET);
    
        rcu_interrupt_flag_clear(RCU_INT_FLAG_HXTALSTB_CLR);
    }
#endif
}

__INLINE_STATIC_ void System_Init(void)
{
    LL_RCC_ClocksTypeDef RCC_Clocks;
    
    LL_RCC_DeInit();

#if HSE_ENABLE
    //开启外部时钟监控
    rcu_interrupt_enable(RCU_INT_HXTALSTB);                       
    rcu_hxtal_clock_monitor_enable();   
    
    //使能外部时钟
    rcu_osci_on(RCU_HXTAL);
    while(rcu_flag_get(RCU_FLAG_HXTALSTB) != SET);
    
    //使用外部时钟作为PLL输入
    rcu_pllpresel_config(RCU_PLLPRESRC_HXTAL);
    rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL30);//PLL_CLK = 8MHz / 2 * 30 = 120MHz
    rcu_osci_on(RCU_PLL_CK);
    while(rcu_flag_get(RCU_FLAG_PLLSTB) != SET);          
#else
    //使能内部时钟
    LL_RCC_HSI_Enable();
    LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
    while(LL_RCC_HSI_IsReady() != SET);
    
    LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
    
    /* 配置HSISYS作为系统时钟源 */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS);
#endif

    /* 设置 AHB 分频*/
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    
    /* 设置 APB1 分频*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DBGMCU);
    
    //获取时钟
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    LL_Clocks.Sysclk = RCC_Clocks.SYSCLK_Frequency;
    LL_Clocks.AHBClk = RCC_Clocks.HCLK_Frequency;
    LL_Clocks.APB1Clk = RCC_Clocks.PCLK1_Frequency;
    
    /* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
    LL_SetSystemCoreClock(LL_Clocks.Sysclk);
}

__INLINE_STATIC_ void System_Disable_IRQ(void)
{
    __disable_irq();
}

__INLINE_STATIC_ void System_Enable_IRQ(void)
{
    __enable_irq();
}

__INLINE_STATIC_ __ASM_ u8 A_System_Get_IRQState(void)
{
    MRS    R0, PRIMASK   //获取当前的中断标识
//    EOR   R0, #1         //指令不支持
    BX     LR            //返回结果
}

__INLINE_STATIC_ u8 System_Get_IRQState(void)
{
    /* 对获取结果取反 */
    return !(A_System_Get_IRQState());
}

__INLINE_STATIC_ void System_Set_Sysclk(FW_Sysclk_Enum mode)
{

}

__INLINE_STATIC_ u32 System_Get_Sysclk(void)
{
    return LL_Clocks.Sysclk;
}

__INLINE_STATIC_ void System_Set_VectorTable(FW_Vector_Table_Enum vector, u32 offset)
{
    if(vector == FW_Vector_Table_RAM)
    {
        SCB->VTOR = SRAM_BASE | offset;
    }
    else
    {
        SCB->VTOR = FLASH_BASE | offset;
    }
}

__INLINE_STATIC_ void System_Soft_Reset(void)
{
    NVIC_SystemReset();
}

__INLINE_STATIC_ FW_RST_Source_Enum System_Get_RSTSource(void)
{
    if(LL_RCC_IsActiveFlag_IWDGRST() == SET)  return FW_RST_Source_IWDG;
    if(LL_RCC_IsActiveFlag_OBLRST() == SET)  return FW_RST_Source_BOR;
    if(LL_RCC_IsActiveFlag_PINRST() == SET)  return FW_RST_Source_Pin;
    if(LL_RCC_IsActiveFlag_PWRRST() == SET)  return FW_RST_Source_POR;
    if(LL_RCC_IsActiveFlag_SFTRST() == SET)  return FW_RST_Source_SOFT;
    return FW_RST_Source_Other;
}

__INLINE_STATIC_ void System_Clear_RSTSource(void)
{
    LL_RCC_ClearResetFlags();
}




/* System Driver */
const static FW_System_Driver_Type Driver = 
{
    .Init            = System_Init,
    .Disable_IRQ     = System_Disable_IRQ,
    .Enable_IRQ      = System_Enable_IRQ,
    .Get_IRQState    = System_Get_IRQState,
    .Set_Sysclk      = System_Set_Sysclk,
    .Get_Sysclk      = System_Get_Sysclk,
    .Set_VectorTable = System_Set_VectorTable,
    .Soft_Reset      = System_Soft_Reset,
    .Get_RSTSource   = System_Get_RSTSource,
    .Clear_RSTSource = System_Clear_RSTSource,
};
FW_DRIVER_REGIST("ll->system", &Driver, System);

