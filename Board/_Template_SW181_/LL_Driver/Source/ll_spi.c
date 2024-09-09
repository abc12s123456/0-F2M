#include "ll_include.h"
#include "ll_gpio.h"

#include "fw_spi.h"
#include "fw_gpio.h"
#include "fw_print.h"

#include "ll_system.h"


#define SPIx(name)(\
(Isdev(name, "spi0")) ? SPI0 :\
(Isdev(name, "spi1")) ? SPI1 :\
(SPI_TypeDef *)INVALUE)


extern FW_SPI_Type SPI0_Device;
extern FW_SPI_Type SPI1_Device;
extern FW_SPI_Type SPI2_Device;


__INLINE_STATIC_ u8 SPI_IRQHandler(FW_SPI_Type *dev)
{
//    u32 spi = (u32)dev->SPIx;
//    
//    if(spi_i2s_interrupt_flag_get(spi, SPI_I2S_INT_FLAG_TBE) == SET)
//    {
//        FW_SPI_TX_ISR(dev);
//    }
//    
//    else if(spi_i2s_interrupt_flag_get(spi, SPI_I2S_INT_FLAG_RBNE) == SET)
//    {
//        FW_SPI_RX_ISR(dev);
//    }
//    
//    else
//    {
//        /**/
//        return False;
//    }
//    
    return True;
}


__LI_ void SPI0_IRQHandler(void)
{
    FW_DEVICE_LIH(&SPI0_Device, SPI_IRQHandler);
}

__LI_ void SPI1_IRQHandler(void)
{
    FW_DEVICE_LIH(&SPI1_Device, SPI_IRQHandler);
}

__LI_ void SPI2_IRQHandler(void)
{
    FW_DEVICE_LIH(&SPI2_Device, SPI_IRQHandler);
}

__INLINE_STATIC_ void LL_SPI_IO_Init(FW_SPI_Type *dev)
{
    SPI_TypeDef *spi = dev->SPIx;
    
    u16 mosi = dev->MOSI_Pin;
    u16 miso = dev->MISO_Pin;
    u16 sck = dev->SCK_Pin;
    u16 cs = dev->CS_Pin;
    
    u32 port_pin_mosi, port_pin_miso, port_pin_sck;
    
    if(spi == SPI0)
    {
        if(mosi == PA10) port_pin_mosi = PORTA_PIN10_SPI0_MOSI;
        else if(mosi == PA14) port_pin_mosi = PORTA_PIN14_SPI0_MOSI;
        else  goto Setup_Error;
            
        if(miso == PA9)  port_pin_miso = PORTA_PIN9_SPI0_MISO;
        else if(miso == PA13)  port_pin_miso = PORTA_PIN13_SPI0_MISO;
        else  goto Setup_Error;
            
        if(sck == PA11)  port_pin_sck = PORTA_PIN11_SPI0_SCLK;
        else if(sck == PA15)  port_pin_sck = PORTA_PIN15_SPI0_SCLK;
        else  goto Setup_Error;
    }
    else if(spi == SPI1)
    {
        if(mosi == PC6) port_pin_mosi = PORTC_PIN6_SPI1_MOSI;
        else  goto Setup_Error;
            
        if(miso == PC5)  port_pin_miso = PORTC_PIN5_SPI1_MISO;
        else  goto Setup_Error;
            
        if(sck == PC7)  port_pin_sck = PORTC_PIN7_SPI1_SCLK;
        else  goto Setup_Error;
    }
    else
    {
        Setup_Error:
        /* 配置错误 */
        while(1);
    }
    
    PORT_Init(GPIO_PORTx(mosi), GPIO_PINx(mosi), port_pin_mosi, 0);
    PORT_Init(GPIO_PORTx(miso), GPIO_PINx(miso), port_pin_miso, 1);
    PORT_Init(GPIO_PORTx(sck), GPIO_PINx(sck), port_pin_sck, 0);
    
    GPIO_Init(GPIOx(cs), GPIO_PINx(cs), 1, 0, 0, 0);
}

__INLINE_STATIC_ u32  LL_SPI_GetDIV(FW_SPI_Type *dev)
{
    u32 baudrate = dev->Baudrate;
    u32 clk = LL_Clocks.APBClk;
    
    if(baudrate >= clk / 4)  return SPI_CLKDIV_4;
    if(baudrate >= clk / 8)  return SPI_CLKDIV_8;
    if(baudrate >= clk / 16)  return SPI_CLKDIV_16;
    if(baudrate >= clk / 32)  return SPI_CLKDIV_32;
    if(baudrate >= clk / 64)  return SPI_CLKDIV_64;
    if(baudrate >= clk / 128)  return SPI_CLKDIV_128;
    if(baudrate >= clk / 256)  return SPI_CLKDIV_256;
    
    return SPI_CLKDIV_512;
}


#define RCU_SPIx(spi)(\
(spi == SPI0) ? RCU_SPI0 :\
(spi == SPI1) ? RCU_SPI1 :\
(spi == SPI2) ? RCU_SPI2 :\
(rcu_periph_enum)INVALUE)

#define SPIx_CLK(spi)(\
(spi == SPI0) ? LL_Clocks.APB2Clk :\
(spi == SPI1) ? LL_Clocks.APB1Clk :\
(spi == SPI2) ? LL_Clocks.APB1Clk :\
INVALUE)


__INLINE_STATIC_ void LL_SPI_Init(FW_SPI_Type *dev)
{
	SPI_InitStructure SPI_initStruct;
	
    char *name = FW_Device_GetName(dev);
    SPI_TypeDef *spi = SPIx(name);
    
    u32 div, edge, level;
    
    u8 trm;
    
    FW_SPI_SetPort(dev, spi);
    
    div = LL_SPI_GetDIV(dev);
    edge = (dev->Clock_Phase == FW_SPI_ClockPhase_Edge1) ?
           SPI_FIRST_EDGE : SPI_SECOND_EDGE;
    level = (dev->Clock_Polarity == FW_SPI_ClockPolarity_H) ?
            SPI_HIGH_LEVEL : SPI_LOW_LEVEL;
    
    LL_SPI_IO_Init(dev);
    
    trm = FW_SPI_GetTRM(dev, TOR_TX);
    if(trm == TRM_DMA)
    {
    
    }
    else if(trm == TRM_INT)
    {
    
    }
    else
    {
    
    }
    
    trm = FW_SPI_GetTRM(dev, TOR_RX);
    if(trm == TRM_DMA)
    {
    
    }
    else if(trm == TRM_INT)
    {
    
    }
    else
    {
    
    }
    
	SPI_initStruct.clkDiv        = div;
	SPI_initStruct.FrameFormat   = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge    = edge;
	SPI_initStruct.IdleLevel     = level;
	SPI_initStruct.WordSize      = 8;
	SPI_initStruct.Master        = 1;
	SPI_initStruct.RXHFullIEn    = 0;
	SPI_initStruct.TXEmptyIEn    = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(spi, &SPI_initStruct);
    
    SPI_Open(spi);
}

__INLINE_STATIC_ void LL_SPI_CTL(FW_SPI_Type *dev, u8 state)
{
    SPI_TypeDef *spi = dev->SPIx;
    state ? SPI_Open(spi) : SPI_Close(spi);
}

__INLINE_STATIC_ void LL_SPI_TX_CTL(FW_SPI_Type *dev, u8 state)
{
    SPI_TypeDef *spi = dev->SPIx;
    u8 trm = FW_SPI_GetTRM(dev, TOR_TX);
    
    (void)spi;
    
    if(trm == TRM_DMA)
    {
    
    }
    else if(trm == TRM_INT)
    {
    
    }
    else
    {
        LL_SPI_CTL(dev, state);
    }
}

__INLINE_STATIC_ void LL_SPI_RX_CTL(FW_SPI_Type *dev, u8 state)
{
    SPI_TypeDef *spi = dev->SPIx;
    u8 trm = FW_SPI_GetTRM(dev, TOR_RX);
    
    (void)spi;
    
    if(trm == TRM_DMA)
    {
    
    }
    else if(trm == TRM_INT)
    {
    
    }
    else
    {
        LL_SPI_CTL(dev, state);
    }
}

__INLINE_STATIC_ void LL_SPI_TX_Byte(FW_SPI_Type *dev, u8 value)
{
//    spi_i2s_data_transmit((u32)dev->SPIx, value);
    SPI_Write((SPI_TypeDef *)dev->SPIx, value);
}

__INLINE_STATIC_ u8   LL_SPI_RX_Byte(FW_SPI_Type *dev)
{
//    return spi_i2s_data_receive((u32)dev->SPIx);
    return SPI_Read((SPI_TypeDef *)dev->SPIx);
}

__INLINE_STATIC_ u8   LL_SPI_Wait_TC(FW_SPI_Type *dev)
{
    return SPI_IsTXFull((SPI_TypeDef *)dev->SPIx);
    
}

__INLINE_STATIC_ u8   LL_SPI_Wait_RC(FW_SPI_Type *dev)
{
    return SPI_IsRXEmpty((SPI_TypeDef *)dev->SPIx);
}




/* Native SPI Driver */
__CONST_STATIC_ FW_SPI_Driver_Type HSPI_Driver =
{
    .Init    = LL_SPI_Init,
    
    .CTL     = LL_SPI_CTL,
    .TX_CTL  = LL_SPI_TX_CTL,
    .RX_CTL  = LL_SPI_RX_CTL,
    
    .TX_Byte = LL_SPI_TX_Byte,
    .RX_Byte = LL_SPI_RX_Byte,
    
    .Wait_TC = LL_SPI_Wait_TC,
    .Wait_RC = LL_SPI_Wait_RC,
};
FW_DRIVER_REGIST("ll->spi", &HSPI_Driver, HSPI);


/* Native SPI Device */
static FW_SPI_Type SPI0_Device;
static void SPI0_Config(void *dev)
{
    FW_Device_SetDriver(&SPI0_Device, (void *)&HSPI_Driver);
}
FW_DEVICE_STATIC_REGIST("spi0", &SPI0_Device, SPI0_Config, SPI0);

static FW_SPI_Type SPI1_Device;
static void SPI1_Config(void *dev)
{
    FW_Device_SetDriver(&SPI1_Device, (void *)&HSPI_Driver);
}
FW_DEVICE_STATIC_REGIST("spi1", &SPI1_Device, SPI1_Config, SPI1);

static FW_SPI_Type SPI2_Device;
static void SPI2_Config(void *dev)
{
    FW_Device_SetDriver(&SPI2_Device, (void *)&HSPI_Driver);
}
FW_DEVICE_STATIC_REGIST("spi2", &SPI2_Device, SPI2_Config, SPI2);

