#include "ll_include.h"
#include "ll_system.h"

#include "fw_system.h"


LL_Clocks_Type LL_Clocks;
static u8 Sysclk_Source = FW_Sysclk_Source_Unknown;


/* 当外部时钟出现故障时，会触发NMI中断；可以在这里进行时钟切换 */
__LI_ void NMI_Handler(void)
{
    #if HSE_EN
    if(crm_interrupt_flag_get(CRM_CLOCK_FAILURE_INT) == SET)
    {
        Sysclk_Source = FW_Sysclk_Source_HSI;
        crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
        while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET);
        crm_pll_config(CRM_PLL_SOURCE_HICK, 36, 1, CRM_PLL_FR_1);
        
        crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
        while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET);
        
        crm_flag_clear(CRM_CLOCK_FAILURE_INT_FLAG);
    }
    #endif
}


static void System_Init(void)
{
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    
    crm_reset();
    
    #if HSE_EN
    Sysclk_Source = FW_Sysclk_Source_HSE;
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);
    while(crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET);
    crm_pll_config(CRM_PLL_SOURCE_HEXT, 36, 1, CRM_PLL_FR_1);
    #else
    Sysclk_Source = FW_Sysclk_Source_HSI;
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
    while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET);
    crm_pll_config(CRM_PLL_SOURCE_HICK, 36, 1, CRM_PLL_FR_1);
    #endif
    
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
    while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET);
    
    crm_ahb_div_set(CRM_AHB_DIV_1);
    crm_apb2_div_set(CRM_APB2_DIV_2);
    crm_apb1_div_set(CRM_APB1_DIV_2);
    
    crm_auto_step_mode_enable(TRUE);
    
    crm_sysclk_switch(CRM_SCLK_PLL);
    while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
    
    crm_auto_step_mode_enable(FALSE);
    
    system_core_clock_update();
    
    LL_Clocks.Sysclk = 288000000;
    LL_Clocks.AHBClk = LL_Clocks.Sysclk;
    LL_Clocks.APB1Clk = LL_Clocks.Sysclk / 2;
    LL_Clocks.APB2Clk = LL_Clocks.Sysclk / 2;
}

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

__INLINE_STATIC_ u8  System_Get_SysclkSource(void)
{
    return Sysclk_Source;
}

__INLINE_STATIC_ void System_Set_VectorTable(FW_Vector_Table_Enum vector, u32 offset)
{
    (vector == FW_Vector_Table_Flash) ?\
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, offset) :
    nvic_vector_table_set(NVIC_VECTTAB_RAM, offset);
}

__INLINE_STATIC_ void System_Soft_Reset(void)
{
    NVIC_SystemReset();
}

__INLINE_STATIC_ FW_RST_Source_Enum System_Get_RSTSource(void)
{
    if(crm_flag_get(CRM_WDT_RESET_FLAG) == SET)        return FW_RST_Source_IWDG;
    if(crm_flag_get(CRM_WWDT_RESET_FLAG) == SET)       return FW_RST_Source_WWDG;
    if(crm_flag_get(CRM_NRST_RESET_FLAG) == SET)       return FW_RST_Source_Pin;
    if(crm_flag_get(CRM_POR_RESET_FLAG) == SET)        return FW_RST_Source_POR;
    if(crm_flag_get(CRM_SW_RESET_FLAG) == SET)         return FW_RST_Source_SOFT;
    if(crm_flag_get(CRM_LOWPOWER_RESET_FLAG) == SET)   return FW_RST_Source_LPW;
    return FW_RST_Source_Other;
}

__INLINE_STATIC_ void System_Clear_RSTSource(void)
{
    crm_flag_clear(CRM_WDT_RESET_FLAG);
    crm_flag_clear(CRM_WWDT_RESET_FLAG);
    crm_flag_clear(CRM_NRST_RESET_FLAG);
    crm_flag_clear(CRM_POR_RESET_FLAG);
    crm_flag_clear(CRM_SW_RESET_FLAG);
    crm_flag_clear(CRM_LOWPOWER_RESET_FLAG);
}




/* System Driver */
__CONST_STATIC_ FW_System_Driver_Type Driver = 
{
    .Init            = System_Init,
    .Disable_IRQ     = System_Disable_IRQ,
    .Enable_IRQ      = System_Enable_IRQ,
    .Get_IRQState    = System_Get_IRQState,
    .Set_Sysclk      = System_Set_Sysclk,
    .Get_Sysclk      = System_Get_Sysclk,
    .Get_SysclkSource = System_Get_SysclkSource,
    .Set_VectorTable = System_Set_VectorTable,
    .Soft_Reset      = System_Soft_Reset,
    .Get_RSTSource   = System_Get_RSTSource,
    .Clear_RSTSource = System_Clear_RSTSource,
};
FW_DRIVER_REGIST("ll->system", &Driver, System);

