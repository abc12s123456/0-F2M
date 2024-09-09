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


#if HSE_EN
__INLINE_STATIC_ void switchToRC24MHz(void)
{
	SYS->HRCCR = (1 << SYS_HRCCR_EN_Pos) |
				 (0 << SYS_HRCCR_DBL_Pos);		//HRC = 24MHz
	
	SYS->CLKSEL &= ~SYS_CLKSEL_HFCK_Msk;		//HFCK  <=  HRC
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);	//SYS_CLK  <= HFCK
}

__INLINE_STATIC_ void switchToXTAL(void)
{
	uint32_t i;
	
	PORT_Init(PORTC, PIN1, PORTC_PIN1_XTAL_IN, 0);
	PORT_Init(PORTC, PIN0, PORTC_PIN0_XTAL_OUT, 0);
	
	SYS->XTALCR = (1 << SYS_XTALCR_EN_Pos);
	
	for(i = 0; i < 22118; i++);
	
	SYS->CLKSEL |= (1 << SYS_CLKSEL_LFCK_Pos);	//LFCK  <=  XTAL
	SYS->CLKSEL &= ~(1 << SYS_CLKSEL_SYS_Pos);	//SYS_CLK  <= LFCK
}
#else
__INLINE_STATIC_ void switchToRC32KHz(void)
{
	SYS->LRCCR = (1 << SYS_LRCCR_EN_Pos);
	
	SYS->CLKSEL &= ~(1 << SYS_CLKSEL_LFCK_Pos);	//LFCK  <=  LRC
	SYS->CLKSEL &= ~(1 << SYS_CLKSEL_SYS_Pos);	//SYS_CLK  <= LFCK
}

__INLINE_STATIC_ void switchToRC48MHz(void)
{
	SYS->HRCCR = (1 << SYS_HRCCR_EN_Pos) |
				 (1 << SYS_HRCCR_DBL_Pos);		//HRC = 48MHz		
	
	SYS->CLKSEL &= ~SYS_CLKSEL_HFCK_Msk;		//HFCK  <=  HRC
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);	//SYS_CLK  <= HFCK
}
#endif




static void System_Init(void)
{
    u32 i;
    
    SYS->CLKEN |= (1 << SYS_CLKEN_OSC_Pos);
    
#if HSE_EN
    if((SYS->CLKSEL & SYS_CLKSEL_SYS_Msk) == 0)
    {
        switchToRC24MHz();
    }
    switchToXTAL();
#else
    /* 当前时钟是高频RC，修改高频RC时钟频率时需要先切到一个稳定时钟源 */
    if(SYS->CLKSEL & SYS_CLKSEL_SYS_Msk)
    {
        switchToRC32KHz();
    }
    switchToRC48MHz();
#endif    
    
    /* 等待时钟稳定 */
    for(i = 0; i < 10000; i++);
    
    SystemCoreClock = 48000000;
    
    LL_Clocks.Sysclk = 48000000;
    LL_Clocks.AHBClk = 48000000;
    LL_Clocks.APBClk = 48000000;
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
__INLINE_STATIC_ __ASM_ u8 A_System_Get_IRQState(void)
{
    MRS    R0, PRIMASK
//    EOR    R0, #1        //有中断触发时，读取结果为0，需要取反返回
    BX     LR
}

__INLINE_STATIC_ u8   System_Get_IRQState(void)
{
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

}

__INLINE_STATIC_ void System_Soft_Reset(void)
{
    NVIC_SystemReset();
}

__INLINE_STATIC_ FW_RST_Source_Enum System_Get_RSTSource(void)
{
    if((SYS->RSTSR & 0x01) == 0)  return FW_RST_Source_POR;
    if((SYS->RSTSR & 0x02) == 0)  return FW_RST_Source_IWDG;
    return FW_RST_Source_Other;
}

__INLINE_STATIC_ void System_Clear_RSTSource(void)
{
    SYS->RSTSR |= 0x03;
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
    .Set_VectorTable = System_Set_VectorTable,
    .Soft_Reset      = System_Soft_Reset,
    .Get_RSTSource   = System_Get_RSTSource,
    .Clear_RSTSource = System_Clear_RSTSource,
};
FW_DRIVER_REGIST("ll->system", &Driver, System);

