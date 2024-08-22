#include "ll_include.h"
#include "ll_system.h"

#include "fw_system.h"


LL_Clocks_Type LL_Clocks;


//当外部时钟出现故障时，会触发NMI中断；可以在这里进行时钟切换
__LI_ void NMI_Handler(void)
{
#if HSE_EN
//    if(rcu_interrupt_flag_get(RCU_INT_FLAG_HXTALSTB) == SET)
//    {
//        //使能内部时钟
//        rcu_osci_on(RCU_IRC8M);
//        while(rcu_flag_get(RCU_FLAG_IRC8MSTB) != SET);
//        
//        //使用内部时钟作为PLL输入
//        rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2, RCU_PLL_MUL30);  //PLL_CLK = 8MHz / 2 * 30 = 120MHz
//        rcu_osci_on(RCU_PLL_CK);
//        while(rcu_flag_get(RCU_FLAG_PLLSTB) != SET);
//    
//        rcu_interrupt_flag_clear(RCU_INT_FLAG_HXTALSTB_CLR);
//    }
#endif
}

__INLINE_STATIC_ void System_Init(void)
{
    LL_RCC_ClocksTypeDef clock;
    
//    LL_RCC_DeInit();
    
    #if HSE_EN
    /* Enable HSE oscillator */
    LL_RCC_HSE_EnableBypass();
    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != SET);
    
    //sysclk = HSE / PLLM * PLLN / PLLP (max = 84M)
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_4);
    #else
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != SET);
    
    LL_RCC_HSI_SetCalibTrimming(16);
    
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, (84 * 4), LL_RCC_PLLP_DIV_4);
    #endif
    
    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    
    /* Main PLL configuration and activation */
    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != SET);
    
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);
    
    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* Set APB1 & APB2 prescaler */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    
    LL_RCC_GetSystemClocksFreq(&clock);
    LL_Clocks.Sysclk = clock.SYSCLK_Frequency;
    LL_Clocks.AHBClk = clock.HCLK_Frequency;
    LL_Clocks.APB1Clk = clock.PCLK1_Frequency;
    LL_Clocks.APB2Clk = clock.PCLK2_Frequency;
    
    SystemCoreClock = 84000000;
}

//{
//    NVIC_SetPriorityGrouping(NVIC_PRIGROUP_PRE2_SUB2); //设置中断优先级组
//    fmc_wscnt_set(WS_WSCNT_0);                         //片上Flash 0等待读写

//    rcu_deinit();
//    
//    //调试端口只使用SWD
//    rcu_periph_clock_enable(RCU_AF);
//    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
//    
//    rcu_periph_clock_enable(RCU_CRC);                  //使能CRC

//#if HSE_EN
//    //开启外部时钟监控
//    rcu_interrupt_enable(RCU_INT_HXTALSTB);                       
//    rcu_hxtal_clock_monitor_enable();   
//    
//    //使能外部时钟
//    rcu_osci_on(RCU_HXTAL);
//    while(rcu_flag_get(RCU_FLAG_HXTALSTB) != SET);
//    
//    //使用外部时钟作为PLL输入
//    rcu_pllpresel_config(RCU_PLLPRESRC_HXTAL);
//    rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL15);//PLL_CLK = 8MHz * 15 = 120MHz
//    rcu_osci_on(RCU_PLL_CK);
//    while(rcu_flag_get(RCU_FLAG_PLLSTB) != SET);          
//#else
//    //使能内部时钟
//    rcu_osci_on(RCU_IRC8M);
//    while(rcu_flag_get(RCU_FLAG_IRC8MSTB) != SET);
//    
//    //使用内部时钟作为PLL输入
//    rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2, RCU_PLL_MUL30);  //PLL_CLK = 8MHz / 2 * 30 = 120MHz
//    rcu_osci_on(RCU_PLL_CK);
//    while(rcu_flag_get(RCU_FLAG_PLLSTB) != SET);
//#endif

//    //使用PLL作为系统时钟输入
//    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
//    
//    //时钟总线配置
//    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);     //AHB
//    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV2);   //APB1
//    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);   //APB2
//    
//    //获取系统时钟
//    LL_Clocks.Sysclk = rcu_clock_freq_get(CK_SYS);
//    LL_Clocks.AHBClk = rcu_clock_freq_get(CK_AHB);
//    LL_Clocks.APB1Clk = rcu_clock_freq_get(CK_APB1);
//    LL_Clocks.APB2Clk = rcu_clock_freq_get(CK_APB2);
//}

__INLINE_STATIC_ void System_Disable_IRQ(void)
{
    __disable_irq();
}

__INLINE_STATIC_ void System_Enable_IRQ(void)
{
    __enable_irq();
}

/* 获取当前的中断状态：0,没有中断触发；1,有中断触发 */
__INLINE_STATIC_ __ASM_ u8 System_Get_IRQState(void)
{
    MRS    R0, PRIMASK
    EOR    R0, #1        //有中断触发时，读取结果为0，需要取反返回
    BX     LR
}

__INLINE_STATIC_ void System_Set_Sysclk(FW_Sysclk_Enum mode)
{

}

__INLINE_STATIC_ u32 System_Get_Sysclk(void)
{
    return LL_Clocks.Sysclk;
}

//__INLINE_STATIC_ void System_Set_VectorTable(FW_Vector_Table_Enum vector, u32 offset)
//{
//    (vector == FW_Vector_Table_Flash) ?\
//    nvic_vector_table_set(NVIC_VECTTAB_FLASH, offset) :
//    nvic_vector_table_set(NVIC_VECTTAB_RAM, offset);
//}

//__INLINE_STATIC_ void System_Soft_Reset(void)
//{
//    NVIC_SystemReset();
//}

//__INLINE_STATIC_ FW_RST_Source_Enum System_Get_RSTSource(void)
//{
//    if(rcu_flag_get(RCU_FLAG_FWDGTRST) == SET)  return FW_RST_Source_IWDG;
//    if(rcu_flag_get(RCU_FLAG_EPRST) == SET)     return FW_RST_Source_Pin;
//    if(rcu_flag_get(RCU_FLAG_PORRST) == SET)    return FW_RST_Source_POR;         //默认为上电复位
//    if(rcu_flag_get(RCU_FLAG_SWRST) == SET)     return FW_RST_Source_SOFT;
//    if(rcu_flag_get(RCU_FLAG_WWDGTRST) == SET)  return FW_RST_Source_WWDG;
//    if(rcu_flag_get(RCU_FLAG_LPRST) == SET)     return FW_RST_Source_LPW;
//    return FW_RST_Source_Other;
//}

//__INLINE_STATIC_ void System_Clear_RSTSource(void)
//{
//    rcu_all_reset_flag_clear();
//}




/* System Driver */
__CONST_STATIC_ FW_System_Driver_Type Driver = 
{
    .Init            = System_Init,
    .Disable_IRQ     = System_Disable_IRQ,
    .Enable_IRQ      = System_Enable_IRQ,
    .Get_IRQState    = System_Get_IRQState,
    .Set_Sysclk      = System_Set_Sysclk,
    .Get_Sysclk      = System_Get_Sysclk,
//    .Set_VectorTable = System_Set_VectorTable,
//    .Soft_Reset      = System_Soft_Reset,
//    .Get_RSTSource   = System_Get_RSTSource,
//    .Clear_RSTSource = System_Clear_RSTSource,
};
FW_DRIVER_REGIST("ll->system", &Driver, System);

