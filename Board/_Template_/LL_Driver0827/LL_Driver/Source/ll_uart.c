#include "ll_include.h"
#include "ll_gpio.h"

#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_print.h"


#define UARTx(name)(\
(Isdev(name, "uart0")) ? USART0 :\
(Isdev(name, "uart1")) ? USART1 :\
(Isdev(name, "uart2")) ? USART2 :\
(Isdev(name, "uart3")) ? UART3 :\
(Isdev(name, "uart4")) ? UART4 :\
(u32)INVALUE)

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

#define UARTx_IRQn(uart)(\
(uart == USART0) ? USART0_IRQn :\
(uart == USART1) ? USART1_IRQn :\
(uart == USART2) ? USART2_IRQn :\
(uart == UART3) ? UART3_IRQn :\
(uart == UART4) ? UART4_IRQn :\
(IRQn_Type)INVALUE)


extern FW_UART_Type USART0_Device;
extern FW_UART_Type USART1_Device;
extern FW_UART_Type USART2_Device;
extern FW_UART_Type UART3_Device;
extern FW_UART_Type UART4_Device;


__INLINE_STATIC_ u8 UART_IRQHandler(FW_UART_Type *dev)
{
    u32 uart = (u32)dev->UARTx;
    
    if(usart_interrupt_flag_get(uart, USART_INT_FLAG_RBNE) == SET)
    {
        FW_UART_RX_ISR(dev);
        usart_interrupt_flag_clear(uart, USART_INT_FLAG_RBNE);
    }
    else if(usart_interrupt_flag_get(uart, USART_INT_FLAG_IDLE) == SET)
    {
        FW_UART_RC_ISR(dev);
        usart_interrupt_flag_clear(uart, USART_INT_FLAG_IDLE);
    }
    
    else if(usart_interrupt_flag_get(uart, USART_INT_FLAG_TBE) == SET)
    {
        FW_UART_TX_ISR(dev);
        usart_interrupt_flag_clear(uart, USART_INT_FLAG_TBE);
    }
    else if(usart_interrupt_flag_get(uart, USART_INT_FLAG_TC) == SET)
    {
        FW_UART_TC_ISR(dev);
        usart_interrupt_flag_get(uart, USART_INT_FLAG_TC);
    }
    
    else
    {
        usart_interrupt_flag_clear(uart, USART_INT_FLAG_ERR_ORERR);
        
        /* 软件读STAT0,再读DATA，清各种错误异常中断标志 */
        USART_STAT0(uart);
        USART_DATA(uart);
        
        /* 该设备没有触发任何中断 */
        return False;
    }
    
    /* 该设备触发了中断 */
    return True;
}
/**/

__LI_ void USART0_IRQHandler(void)
{
    FW_DEVICE_LIH(&USART0_Device, UART_IRQHandler);
}

__LI_ void USART1_IRQHandler(void)
{
    FW_DEVICE_LIH(&USART1_Device ,UART_IRQHandler);
}

__LI_ void USART2_IRQHandler(void)
{
    FW_DEVICE_LIH(&USART2_Device ,UART_IRQHandler);
}

__LI_ void UART3_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART3_Device ,UART_IRQHandler);
}

__LI_ void UART4_IRQHandler(void)
{
    FW_DEVICE_LIH(&UART4_Device ,UART_IRQHandler);
}


/* GD32F303x中只有USART0/1/2具有重映射功能 */
__INLINE_ static void UART_IO_Init(FW_UART_Type *dev)
{
    u32 uart = (u32)dev->UARTx;
    
    u16 TX_Pin = dev->TX_Pin;
    u16 RX_Pin = dev->RX_Pin;
    
    FW_GPIO_Init(TX_Pin, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
    FW_GPIO_Init(RX_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
    
    if(uart == USART0)
    {
        if(TX_Pin == PA9 || RX_Pin == PA10){}
        else if(TX_Pin == PB6 || RX_Pin == PB7)
        {
            gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
        }
        else{}
    }
    else if(uart == USART1)
    {
        if(TX_Pin == PA2 || RX_Pin == PA3){}
        else if(TX_Pin == PD5 || RX_Pin == PD6)
        {
            gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);
        }
        else{}
    }
    else if(uart == USART2)
    {
        if(TX_Pin == PB10 || RX_Pin == PB11){}
        else if(TX_Pin == PC10 || RX_Pin == PC11)
        {
            gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP, ENABLE);
        }
        else if(TX_Pin == PD8 || RX_Pin == PD9)
        {
            gpio_pin_remap_config(GPIO_USART2_FULL_REMAP, ENABLE);
        }
        else{}
    }
    else if(uart == UART3)
    {
        if(TX_Pin == PC10 || RX_Pin == PC11){}
    }
    else if(uart == UART4)
    {
        if(TX_Pin == PC12 || RX_Pin == PD2){}
    }
    else
    {
        while(1);                                 //IO配置错误
    }
}
/**/

__INLINE_STATIC_ void UART_DeInit(FW_UART_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    u32 uart = (u32)UARTx(name);
    
    FW_UART_SetPort(dev, (void *)uart);
    
    usart_deinit(uart);
}
/**/

__INLINE_STATIC_ void UART_Init(FW_UART_Type *dev)
{
    dma_parameter_struct dma_init_struct;
    
    char *name = FW_Device_GetName(dev);
    u32 uart = (u32)UARTx(name);
    
    FW_UART_SetPort(dev, (void *)uart);
    
    u32 dma = DMAx(uart);
    dma_channel_enum dma_tx_ch = DMA_CHx(uart, TOR_TX);
    dma_channel_enum dma_rx_ch = DMA_CHx(uart, TOR_RX);
    
    u8 trm;
    
    rcu_periph_clock_enable(RCU_UARTx(uart));
    
    usart_deinit(uart);
    
    UART_IO_Init(dev);
    
    trm = FW_UART_GetTRM(dev, TOR_TX);
    if(trm == TRM_DMA)
    {
        rcu_periph_clock_enable(RCU_DMAx(uart));
        
        dma_deinit(dma, dma_tx_ch);
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.periph_addr  = (u32)&USART_DATA(uart);
        dma_init_struct.memory_addr  = (u32)FW_UART_GetDMABBase(dev, TOR_TX);
        dma_init_struct.number       = FW_UART_GetDMABSize(dev, TOR_TX);
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.priority     = DMA_PRIORITY_LOW;
        dma_init(dma, dma_tx_ch, &dma_init_struct);
        
        dma_circulation_disable(dma, dma_tx_ch);
        dma_memory_to_memory_disable(dma, dma_tx_ch);
        
        dma_channel_disable(dma, dma_tx_ch);    
        
        /* 初始化完成后，TC中断关闭，发送数据时再打开 */
        nvic_irq_enable(UARTx_IRQn(uart), 3, 3);
        usart_interrupt_disable(uart, USART_INT_TC);
        
        usart_dma_transmit_config(uart, USART_TRANSMIT_DMA_DISABLE);
    }
    else if(trm == TRM_INT)
    {
        /* 初始化完成后，TX中断关闭，发送数据时再打开 */
        nvic_irq_enable(UARTx_IRQn(uart), 3, 3);
        usart_interrupt_disable(uart, USART_INT_TBE);
    }
    else
    {
        usart_flag_clear(uart, USART_FLAG_TBE);
    }
    
    trm = FW_UART_GetTRM(dev, TOR_RX);
    if(trm == TRM_DMA)
    {
        rcu_periph_clock_enable(RCU_DMAx(uart)); //DMA时钟使能
        
        dma_deinit(dma, dma_rx_ch);
        dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.periph_addr  = (u32)&USART_DATA(uart);
        dma_init_struct.memory_addr  = (u32)FW_UART_GetDMABBase(dev, TOR_RX);
        dma_init_struct.number       = FW_UART_GetDMABSize(dev, TOR_RX);
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.priority     = DMA_PRIORITY_LOW;
        dma_init(dma, dma_rx_ch, &dma_init_struct);
        
        dma_circulation_disable(dma, dma_rx_ch);
        dma_channel_enable(dma, dma_rx_ch);
        
        nvic_irq_enable(UARTx_IRQn(uart), 3, 3);
        usart_interrupt_enable(uart, USART_INT_IDLE);
        
        usart_dma_receive_config(uart, USART_RECEIVE_DMA_DISABLE);
    }
    else if(trm == TRM_INT)
    {
        nvic_irq_enable(UARTx_IRQn(uart), 3, 3);
        usart_interrupt_enable(uart, USART_INT_RBNE);
    }
    else
    {
        usart_flag_clear(uart, USART_FLAG_RBNE);
    }
    
    usart_baudrate_set(uart, dev->Baudrate);           //设置波特率
    
    usart_receive_config(uart, USART_RECEIVE_ENABLE);  //默认开接收
    usart_transmit_config(uart, USART_TRANSMIT_ENABLE);//默认开发送
    
    usart_enable(uart);
}
/**/

__INLINE_STATIC_ void UART_CTL(FW_UART_Type *dev, u8 state)
{
    u32 uart = (u32)dev->UARTx;
    state ? usart_enable(uart) : usart_disable(uart);
}
/**/

__INLINE_STATIC_ void UART_TX_CTL(FW_UART_Type *dev, u8 state)
{
    u32 uart = (u32)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_TX);
    
    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(uart);
        dma_channel_enum dma_tx_ch = DMA_CHx(uart, TOR_TX);
        
        if(state)
        {
            usart_interrupt_enable(uart, USART_INT_TC);
            dma_channel_enable(dma, dma_tx_ch);
        }
        else
        {
            usart_interrupt_disable(uart, USART_INT_TC);
            dma_channel_disable(dma, dma_tx_ch);
        }
    }
    else if(trm == TRM_INT)
    {
        state ?\
        usart_interrupt_enable(uart, USART_INT_TBE) :\
        usart_interrupt_disable(uart, USART_INT_TBE);
    }
    else
    {
        state ?\
        usart_transmit_config(uart, USART_TRANSMIT_ENABLE) :\
        usart_transmit_config(uart, USART_TRANSMIT_DISABLE);
    }
}
/**/
    
__INLINE_STATIC_ void UART_RX_CTL(FW_UART_Type *dev, u8 state)
{
    u32 uart = (u32)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_RX);
    
    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(uart);
        dma_channel_enum dma_rx_ch = DMA_CHx(uart, TOR_RX);

        state ?\
        dma_channel_enable(dma, dma_rx_ch) :\
        dma_channel_disable(dma, dma_rx_ch);
    }
    else if(trm == TRM_INT)
    {
        state ?\
        usart_interrupt_enable(uart, USART_INT_RBNE) :\
        usart_interrupt_disable(uart, USART_INT_RBNE);
    }
    else
    {
        state ?\
        usart_receive_config(uart, USART_RECEIVE_ENABLE) :\
        usart_receive_config(uart, USART_RECEIVE_DISABLE);
    }
}
/**/

__INLINE_STATIC_ void UART_TX_Byte(FW_UART_Type *dev, u8 value)
{
    usart_data_transmit((u32)dev->UARTx, value);
}
/**/

__INLINE_STATIC_ u8 UART_RX_Byte(FW_UART_Type *dev)
{
    return (u8)usart_data_receive((u32)dev->UARTx);
}
/**/

__INLINE_STATIC_ u8 UART_Wait_TC(FW_UART_Type *dev)
{
    u32 uart = (u32)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_TX);
    
    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(uart);
        dma_channel_enum dma_tx_ch = DMA_CHx(uart, TOR_TX);
        return dma_flag_get(dma, dma_tx_ch, DMA_FLAG_FTF);
    }
    else
    {
        return usart_flag_get(uart, USART_FLAG_TBE);
    }
}
/**/

__INLINE_STATIC_ u8 UART_Wait_RC(FW_UART_Type *dev)
{
    u32 uart = (u32)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_RX);

    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(uart);
        dma_channel_enum dma_rx_ch = DMA_CHx(uart, TOR_RX);
        return dma_flag_get(dma, dma_rx_ch, DMA_FLAG_FTF);
    }
    else
    {
        return usart_flag_get(uart, USART_FLAG_RBNE);
    }
}
/**/

__INLINE_STATIC_ void UART_Set_TDL(FW_UART_Type *dev, u32 length)
{
    u32 uart = (u32)dev->UARTx;
    u32 dma = DMAx(uart);
    dma_channel_enum dma_tx_ch = DMA_CHx(uart, TOR_TX);

    dma_flag_clear(dma, dma_tx_ch, DMA_FLAG_FTF | DMA_FLAG_HTF);
    
    dma_channel_disable(dma, dma_tx_ch);
    dma_memory_address_config(dma, dma_tx_ch, (u32)FW_UART_GetDMABBase(dev, TOR_TX));
    dma_transfer_number_config(dma, dma_tx_ch, length);
    dma_channel_enable(dma, dma_tx_ch);
}
/**/

__INLINE_STATIC_ u32 UART_Get_RDL(FW_UART_Type *dev)
{
    u32 uart = (u32)dev->UARTx;
    u32 dma = DMAx(uart);
    dma_channel_enum dma_rx_ch = DMA_CHx(uart, TOR_RX);
    u32 size = FW_UART_GetDMABSize(dev, TOR_RX);
    
    dma_flag_clear(dma, dma_rx_ch, DMA_FLAG_FTF | DMA_FLAG_HTF);
    
    u32 num = dma_transfer_number_get(dma, dma_rx_ch);
    dma_memory_address_config(dma, dma_rx_ch, (u32)FW_UART_GetDMABBase(dev, TOR_RX));  //接收完成后，需重新设定memory addr
    
    if(size >= num)
    {
        return (size - num);
    }
    return 0;
}
/**/




/* Native UART Driver */
__CONST_STATIC_ FW_UART_Driver_Type HUART_Driver =
{
    .DeInit  = UART_DeInit,
    .Init    = UART_Init,
    
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
static FW_UART_Type USART0_Device;
static void USART0_Config(void *dev)
{
    FW_Device_SetDriver(&USART0_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart0", &USART0_Device, USART0_Config, USART0);

static FW_UART_Type USART1_Device;
static void USART1_Config(void *dev)
{
    FW_Device_SetDriver(&USART1_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart1", &USART1_Device, USART1_Config, USART1);

static FW_UART_Type USART2_Device;
static void USART2_Config(void *dev)
{
    FW_Device_SetDriver(&USART2_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart2", &USART2_Device, USART2_Config, USART2);

static FW_UART_Type UART3_Device;
static void UART3_Config(void *dev)
{
    FW_Device_SetDriver(&UART3_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart3", &UART3_Device, UART3_Config, UART3);

static FW_UART_Type UART4_Device;
static void UART4_Config(void *dev)
{
    FW_Device_SetDriver(&UART4_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart4", &UART4_Device, UART4_Config, UART4);






#include "fw_debug.h"
#if MODULE_TEST && UART_TEST


void Test(void)
{
    FW_UART_PrintInit(PA9, 9600);
    
    printf("hello world\r\n");
    
    while(1);
}


#endif

