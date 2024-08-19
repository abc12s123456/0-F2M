#include "ll_include.h"
#include "ll_gpio.h"

#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_print.h"


#if defined(UART_MOD_EN) && UART_MOD_EN


#define UARTx(name)(\
(Isdev(name, "huart1")) ? USART1 :\
(USART_TypeDef *)INVALUE)


__INLINE_STATIC_ u8 UART_IRQHandler(FW_UART_Type *dev)
{
    USART_TypeDef *uart = (USART_TypeDef *)dev->UARTx;
    
    if(LL_USART_IsActiveFlag_RXNE(uart) == SET &&
       LL_USART_IsEnabledIT_RXNE(uart) == SET)
    {
        FW_UART_RX_ISR(dev);
        LL_USART_ClearFlag_RXNE(uart);
    }
    else if(LL_USART_IsActiveFlag_IDLE(uart) == SET &&
            LL_USART_IsEnabledIT_IDLE(uart) == SET)
    {
        FW_UART_RC_ISR(dev);
        LL_USART_ClearFlag_IDLE(uart);
    }
    
    else if(LL_USART_IsActiveFlag_TXE(uart) == SET &&
            LL_USART_IsEnabledIT_TXE(uart) == SET)
    {
        FW_UART_TX_ISR(dev);
    }
    else if(LL_USART_IsActiveFlag_TC(uart) == SET &&
            LL_USART_IsEnabledIT_TC(uart) == SET)
    {
        FW_UART_TC_ISR(dev);
        LL_USART_ClearFlag_TC(uart);
    }
    
    else
    {
        /* 清除异常中断标识 */
        LL_USART_ReceiveData8(uart);
        
        LL_USART_ClearFlag_ORE(uart);
        LL_USART_ClearFlag_FE(uart);
        LL_USART_ClearFlag_NE(uart);
        LL_USART_ClearFlag_PE(uart);
        
        /* 该设备没有触发任何中断 */
        return False;
    }
    
    /* 该设备触发了中断 */
    return True;
}


extern FW_UART_Type USART1_Device;

__LI_ void USART1_IRQHandler(void)
{
    FW_DEVICE_LIH(&USART1_Device ,UART_IRQHandler);
}


__INLINE_ static void UART_IO_Init(FW_UART_Type *dev)
{
    u16 TX_Pin = dev->TX_Pin;
    u16 RX_Pin = dev->RX_Pin;
    u8 GPIO_AF_TX, GPIO_AF_RX;
    
    //TX_Pin
    if(TX_Pin == PF1 || TX_Pin == PA7 || TX_Pin == PA10)
    {
        GPIO_AF_TX = LL_GPIO_AF_8;
    }
    else if(TX_Pin == PA2 || TX_Pin == PA9 || TX_Pin == PA14)
    {
        GPIO_AF_TX = LL_GPIO_AF_1;
    }
//    else if(TX_Pin == PA7){GPIO_AF_TX = LL_GPIO_AF_8;}
//    else if(TX_Pin == PA9){GPIO_AF_TX = LL_GPIO_AF_1;}
//    else if(TX_Pin == PA10){GPIO_AF_TX = LL_GPIO_AF_8;}
//    else if(TX_Pin == PA14){GPIO_AF_TX = LL_GPIO_AF_1;}
    else if(TX_Pin == PB6)
    {
        GPIO_AF_TX = LL_GPIO_AF_0;
    }
    else{}
    
    //RX_Pin
    if(RX_Pin == PF0 || RX_Pin == PA8 || RX_Pin == PA9 || RX_Pin == PA13)
    {
        GPIO_AF_RX = LL_GPIO_AF_8;
    }
    else if(RX_Pin == PA3 || RX_Pin == PA10)
    {
        GPIO_AF_RX = LL_GPIO_AF_1;
    }
    else if(RX_Pin == PB2 || RX_Pin == PB7)
    {
        GPIO_AF_RX = LL_GPIO_AF_0;
    }
//    else if(RX_Pin == PA8){GPIO_AF_RX = LL_GPIO_AF_8;}
//    else if(RX_Pin == PA9){GPIO_AF_RX = LL_GPIO_AF_8;}
//    else if(RX_Pin == PA10){GPIO_AF_RX = LL_GPIO_AF_1;}
//    else if(RX_Pin == PA13){GPIO_AF_RX = LL_GPIO_AF_8;}
//    else if(RX_Pin == PB7){GPIO_AF_RX = LL_GPIO_AF_0;}
    else{}
        
    LL_GPIO_PinAFConfig(TX_Pin, GPIO_AF_TX);
    LL_GPIO_PinAFConfig(RX_Pin, GPIO_AF_RX);
        
    FW_GPIO_Init(TX_Pin, FW_GPIO_Mode_AF_Out_PPU, FW_GPIO_Speed_Ultra);
    FW_GPIO_Init(RX_Pin, FW_GPIO_Mode_AF_Out_PPU, FW_GPIO_Speed_Ultra);
}

__INLINE_STATIC_ void UART_DeInit(FW_UART_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    USART_TypeDef * uart = (USART_TypeDef *)UARTx(name);
    
    FW_UART_SetPort(dev, (void *)uart);
    
    LL_USART_DeInit(uart);
}
/**/

__INLINE_STATIC_ void UART_Init(FW_UART_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    USART_TypeDef * uart = (USART_TypeDef *)UARTx(name);
    
    LL_USART_InitTypeDef USART_InitStruct = {0};
    
    FW_UART_SetPort(dev, (void *)uart);
    
    u8 trm;
    
    /* 只有一个USART端口 */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
    
    UART_IO_Init(dev);
    
    trm = FW_UART_GetTRM(dev, TOR_TX);
    if(trm == TRM_INT)
    {
        NVIC_SetPriority(USART1_IRQn, 3);
        NVIC_EnableIRQ(USART1_IRQn);
        LL_USART_DisableIT_TXE(uart);
    }
    else
    {
        LL_USART_EnableDirectionTx(uart);
    }
    
    trm = FW_UART_GetTRM(dev, TOR_RX);
    if(trm == TRM_INT)
    {
        NVIC_SetPriority(USART1_IRQn, 3);
        NVIC_EnableIRQ(USART1_IRQn);
    
        /*使能接收奇偶校验错误中断*/
        LL_USART_EnableIT_PE(uart);

        /*使能接收错误中断*/
        LL_USART_EnableIT_ERROR(uart);
        
        LL_USART_EnableIT_RXNE(uart);
    }
    else
    {
        LL_USART_EnableDirectionRx(uart);
    }
    
    USART_InitStruct.BaudRate            = dev->Baudrate;
    USART_InitStruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits            = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity              = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling        = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(uart, &USART_InitStruct);
    
    LL_USART_ConfigAsyncMode(uart);
    
    LL_USART_Enable(uart);
}
/**/

__INLINE_STATIC_ void UART_CTL(FW_UART_Type *dev, u8 state)
{
    USART_TypeDef *uart = (USART_TypeDef *)dev->UARTx;
    state ? LL_USART_Enable(uart) : LL_USART_Disable(uart);
}
/**/

__INLINE_STATIC_ void UART_TX_CTL(FW_UART_Type *dev, u8 state)
{
    USART_TypeDef *uart = (USART_TypeDef *)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_TX);
    
    if(trm == TRM_INT)
    {
        state ?\
        LL_USART_EnableIT_TXE(uart) :\
        LL_USART_DisableIT_TXE(uart);
    }
    else
    {
        state ?\
        LL_USART_EnableDirectionTx(uart) :\
        LL_USART_DisableDirectionTx(uart);
    }
}
/**/
    
__INLINE_STATIC_ void UART_RX_CTL(FW_UART_Type *dev, u8 state)
{
    USART_TypeDef *uart = (USART_TypeDef *)dev->UARTx;
    u8 trm = FW_UART_GetTRM(dev, TOR_RX);
    
    if(trm == TRM_INT)
    {
        state ?\
        LL_USART_EnableIT_RXNE(uart) :\
        LL_USART_DisableIT_RXNE(uart);
    }
    else
    {
        state ?\
        LL_USART_EnableDirectionRx(uart) :\
        LL_USART_DisableDirectionRx(uart);
    }
}
/**/

__INLINE_STATIC_ void UART_TX_Byte(FW_UART_Type *dev, u8 value)
{
    LL_USART_TransmitData8((USART_TypeDef *)dev->UARTx, value);
}
/**/

__INLINE_STATIC_ u8 UART_RX_Byte(FW_UART_Type *dev)
{
    return LL_USART_ReceiveData8((USART_TypeDef *)dev->UARTx);
}
/**/

__INLINE_STATIC_ u8 UART_Wait_TC(FW_UART_Type *dev)
{
    return LL_USART_IsActiveFlag_TXE((USART_TypeDef *)dev->UARTx);
}
/**/

__INLINE_STATIC_ u8 UART_Wait_RC(FW_UART_Type *dev)
{
    return LL_USART_IsActiveFlag_RXNE((USART_TypeDef *)dev->UARTx);
}
/**/




/* Native UART Driver */
const static FW_UART_Driver_Type HUART_Driver =
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
    
    .Set_TDL = NULL,
    .Get_RDL = NULL,
};
FW_DRIVER_REGIST("ll->uart", &HUART_Driver, HUART);




static FW_UART_Type USART1_Device;
static void USART1_Config(void *dev)
{
    FW_Device_SetDriver(&USART1_Device, (void *)&HUART_Driver);
}
FW_DEVICE_STATIC_REGIST("uart1", &USART1_Device, USART1_Config, USART1);


#endif  /* defined(SUART_MOD_EN) && SUART_MOD_EN */

