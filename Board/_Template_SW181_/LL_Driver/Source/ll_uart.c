#include "ll_include.h"
#include "ll_gpio.h"

#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_print.h"


#define UARTx(name)(\
(Isdev(name, "uart0")) ? UART0 :\
(Isdev(name, "uart1")) ? UART1 :\
(Isdev(name, "uart2")) ? UART2 :\
(Isdev(name, "uart3")) ? UART3 :\
(UART_TypeDef *)INVALUE)

/* UART4不支持DMA */
#define DMAx(uart)(\
(uart == USART0) ? DMA0 :\
(uart == USART1) ? DMA0 :\
(uart == USART2) ? DMA0 :\
(uart == UART3) ? DMA1 :\
INVALUE)

#define DMA_CHx(uart, tr)(\
(uart == USART0 && tr == TOR_TX) ? DMA_CH3 :\
(uart == USART0 && tr == TOR_RX) ? DMA_CH4 :\
(uart == USART1 && tr == TOR_TX) ? DMA_CH6 :\
(uart == USART1 && tr == TOR_RX) ? DMA_CH5 :\
(uart == USART2 && tr == TOR_TX) ? DMA_CH1 :\
(uart == USART2 && tr == TOR_RX) ? DMA_CH2 :\
(uart == UART3 && tr == TOR_TX) ? DMA_CH4 :\
(uart == UART3 && tr == TOR_RX) ? DMA_CH2 :\
(dma_channel_enum)INVALUE)

#define RCU_DMAx(uart)(\
(uart == USART0) ? RCU_DMA0 :\
(uart == USART1) ? RCU_DMA0 :\
(uart == USART2) ? RCU_DMA0 :\
(uart == UART3) ? RCU_DMA1 :\
(rcu_periph_enum)INVALUE)

#define RCU_UARTx(uart)(\
(uart == USART0) ? RCU_USART0 :\
(uart == USART1) ? RCU_USART1 :\
(uart == USART2) ? RCU_USART2 :\
(uart == UART3) ? RCU_UART3 :\
(uart == UART4) ? RCU_UART4 :\
(rcu_periph_enum)INVALUE)


/* 
1、SWM181的中断体系比较灵(奇)活(葩)，但可以由用户在底层定义好，对顶层应用而言，与
   其它大众的中断方式无异;
2、SWM181共计IRQ0~IRQ31个中断向量，其中IRQ0~IRQ15每个只能关联一个外设，IRQ16~IRQ31
   允许关联两个外设；
   
*/
#define IRQx_IRQ(uart)(\
(uart == UART0) ? IRQ0_IRQ :\
(uart == UART1) ? IRQ1_IRQ :\
(uart == UART2) ? IRQ2_IRQ :\
(uart == UART3) ? IRQ3_IRQ :\
(IRQn_Type)INVALUE)

#define IRQ0_15_UARTx(uart)(\
(uart == UART0) ? IRQ0_15_UART0 :\
(uart == UART1) ? IRQ0_15_UART1 :\
(uart == UART2) ? IRQ0_15_UART2 :\
(uart == UART3) ? IRQ0_15_UART3 :\
INVALUE)


extern FW_UART_Type UART0_Device;
extern FW_UART_Type UART1_Device;
extern FW_UART_Type UART2_Device;
extern FW_UART_Type UART3_Device;


__INLINE_STATIC_ u8 UART_IRQHandler(FW_UART_Type *dev)
{
    UART_TypeDef *uart = dev->UARTx;
    
    if(UART_INTRXThresholdStat(uart) == 1)
    {
        FW_UART_RX_ISR(dev);
    }
    
    else if(UART_INTTXThresholdStat(uart) == 1)
    {
        FW_UART_TX_ISR(dev);
    }
    
    else
    {
        return False;
    }
    
    return True;
}
/**/

__LI_ void USART0_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART0_Device, UART_IRQHandler);
}

__LI_ void USART1_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART1_Device ,UART_IRQHandler);
}

__LI_ void USART2_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART2_Device ,UART_IRQHandler);
}

__LI_ void UART3_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART3_Device ,UART_IRQHandler);
}


/* GD32F303x中只有USART0/1/2具有重映射功能 */
__INLINE_ static void LL_UART_IO_Init(FW_UART_Type *dev)
{
    UART_TypeDef *uart = dev->UARTx;
    
    u16 tx_pin = dev->TX_Pin;
    u16 rx_pin = dev->RX_Pin;
    
    u32 func_tx, func_rx;
    
    if(uart == UART0)
    {
        func_tx = FUNMUX_UART0_TXD;
        func_rx = FUNMUX_UART0_RXD;
    }
    else if(uart == UART1)
    {
        func_tx = FUNMUX_UART1_TXD;
        func_rx = FUNMUX_UART1_RXD;
    }
    else if(uart == UART2)
    {
        func_tx = FUNMUX_UART2_TXD;
        func_rx = FUNMUX_UART2_RXD;
    }
    else if(uart == UART3)
    {
        func_tx = FUNMUX_UART3_TXD;
        func_rx = FUNMUX_UART3_RXD;
    }
    else
    {
        /* 配置错误 */
        while(1);
    }
    
    PORT_Init(GPIO_PORTx(tx_pin), GPIO_PINx(tx_pin), func_tx, 0);
    PORT_Init(GPIO_PORTx(rx_pin), GPIO_PINx(rx_pin), func_rx, 1);
}
/**/

__INLINE_STATIC_ void LL_UART_Init(FW_UART_Type *dev)
{
    UART_InitStructure UART_initStruct;
    
    char *name = FW_Device_GetName(dev);
    UART_TypeDef *uart = UARTx(name);
    
    FW_UART_SetPort(dev, (void *)uart);

    u8 trm;
    
    LL_UART_IO_Init(dev);
    
    trm = FW_UART_GetTRM(dev, TOR_TX);
    if(trm == TRM_DMA)
    {
        UART_initStruct.TimeoutIEn     = 0;
    
    }
    else if(trm == TRM_INT)
    {
        UART_initStruct.TXThreshold = 1;
        UART_initStruct.TXThresholdIEn = 1;
        IRQ_Connect(IRQ0_15_UARTx(uart), IRQx_IRQ(uart), 3);
    }
    else
    {
        UART_initStruct.TXThreshold = 1;
        UART_initStruct.TXThresholdIEn = 0;
    }
    
    trm = FW_UART_GetTRM(dev, TOR_RX);
    if(trm == TRM_DMA)
    {
        UART_initStruct.TimeoutIEn     = 0;
    
    }
    else if(trm == TRM_INT)
    {
        UART_initStruct.RXThreshold = 1;
        UART_initStruct.RXThresholdIEn = 1;
        IRQ_Connect(IRQ0_15_UARTx(uart), IRQx_IRQ(uart), 3);
    }
    else
    {
        UART_initStruct.RXThreshold = 1;
        UART_initStruct.RXThresholdIEn = 0;
    }        
    
    
 	UART_initStruct.Baudrate       = dev->Baudrate;
	UART_initStruct.DataBits       = UART_DATA_8BIT;
	UART_initStruct.Parity         = UART_PARITY_NONE;
	UART_initStruct.StopBits       = UART_STOP_1BIT;
 	UART_Init(uart, &UART_initStruct);
}
/**/

__INLINE_STATIC_ void UART_CTL(FW_UART_Type *dev, u8 state)
{
    UART_TypeDef *uart = dev->UARTx;
    state ? UART_Open(uart) : UART_Close(uart);
}
/**/

__INLINE_STATIC_ void UART_TX_CTL(FW_UART_Type *dev, u8 state)
{
    UART_TypeDef *uart = dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_TX);
    
    (void)uart;
    
    if(trm == TRM_DMA)
    {
        
    }
    else if(trm == TRM_INT)
    {
        state ?
        UART_INTTXThresholdEn(uart) :
        UART_INTTXThresholdDis(uart);
    }
    else
    {
        if(state) uart->FIFO |= 1 << UART_FIFO_TXTHR_Pos;
        else  uart->FIFO &= ~(1 << UART_FIFO_TXTHR_Pos);
    }
}
/**/
    
__INLINE_STATIC_ void UART_RX_CTL(FW_UART_Type *dev, u8 state)
{
    UART_TypeDef *uart = dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_RX);
    
    (void)uart;
    
    if(trm == TRM_DMA)
    {
        
    }
    else if(trm == TRM_INT)
    {
        state ?
        UART_INTRXThresholdEn(uart) :
        UART_INTRXThresholdDis(uart);
    }
    else
    {
        if(state) uart->FIFO |= 1 << UART_FIFO_RXTHR_Pos;
        else  uart->FIFO &= ~(1 << UART_FIFO_RXTHR_Pos);
    }
}
/**/

__INLINE_STATIC_ void UART_TX_Byte(FW_UART_Type *dev, u8 value)
{
    UART_WriteByte((UART_TypeDef *)dev->UARTx, value);
}
/**/

__INLINE_STATIC_ u8 UART_RX_Byte(FW_UART_Type *dev)
{
    u32 value;
    if(UART_ReadByte((UART_TypeDef *)dev->UARTx, &value) == 0)
    {
        return value;
    }
    return 0xA5;
}
/**/

__INLINE_STATIC_ u8 UART_Wait_TC(FW_UART_Type *dev)
{
    UART_TypeDef *uart = dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_TX);
    
    if(trm == TRM_DMA)
    {
        
    }
    else
    {
        return UART_IsTXBusy(uart);
    }
    
    return 1;
}
/**/

__INLINE_STATIC_ u8 UART_Wait_RC(FW_UART_Type *dev)
{
    UART_TypeDef *uart = dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_RX);
    
    if(trm == TRM_DMA)
    {
        
    }
    else
    {
        return UART_IsRXFIFOEmpty(uart);
    }
    
    return 1;
}
/**/

__INLINE_STATIC_ void UART_Set_TDL(FW_UART_Type *dev, u32 length)
{

}
/**/

__INLINE_STATIC_ u32 UART_Get_RDL(FW_UART_Type *dev)
{

    return 0;
}
/**/




/* Native UART Driver */
__CONST_STATIC_ FW_UART_Driver_Type HUART_Driver =
{
    .Init    = LL_UART_Init,
    
    .CTL     = UART_CTL,
    .TX_CTL  = UART_TX_CTL,
    .RX_CTL  = UART_RX_CTL,
    
    .TX_Byte = UART_TX_Byte,
    .RX_Byte = UART_RX_Byte,
    
    .Wait_TC = UART_Wait_TC,
    .Wait_RC = UART_Wait_RC,
    
    .Set_TDL = UART_Set_TDL,
    .Get_RDL = UART_Get_RDL,
};
FW_DRIVER_REGIST("ll->uart", &HUART_Driver, HUART);


/* Native UART Device */
static FW_UART_Type UART0_Device;
static void USART0_Config(void *dev)
{
    FW_Device_SetDriver(&UART0_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart0", &UART0_Device, USART0_Config, USART0);

static FW_UART_Type UART1_Device;
static void USART1_Config(void *dev)
{
    FW_Device_SetDriver(&UART1_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart1", &UART1_Device, USART1_Config, USART1);

static FW_UART_Type UART2_Device;
static void USART2_Config(void *dev)
{
    FW_Device_SetDriver(&UART2_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart2", &UART2_Device, USART2_Config, USART2);

static FW_UART_Type UART3_Device;
static void UART3_Config(void *dev)
{
    FW_Device_SetDriver(&UART3_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart3", &UART3_Device, UART3_Config, UART3);




#include "fw_debug.h"
#if MODULE_TEST && UART_TEST


void Test(void)
{
    FW_UART_PrintInit(PA9, 9600);
    
    printf("hello world\r\n");
    
    while(1);
}


#endif

