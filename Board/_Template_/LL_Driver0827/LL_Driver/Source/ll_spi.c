#include "ll_include.h"

#include "fw_spi.h"
#include "fw_gpio.h"
#include "fw_print.h"

#include "ll_system.h"


#define SPIx(name)(\
(Isdev(name, "spi0")) ? SPI0 :\
(Isdev(name, "spi1")) ? SPI1 :\
(Isdev(name, "spi2")) ? SPI2 :\
(u32)INVALUE)


extern FW_SPI_Type SPI0_Device;
extern FW_SPI_Type SPI1_Device;
extern FW_SPI_Type SPI2_Device;


__INLINE_STATIC_ u8 SPI_IRQHandler(FW_SPI_Type *dev)
{
    u32 spi = (u32)dev->SPIx;
    
    if(spi_i2s_interrupt_flag_get(spi, SPI_I2S_INT_FLAG_TBE) == SET)
    {
        FW_SPI_TX_ISR(dev);
    }
    
    else if(spi_i2s_interrupt_flag_get(spi, SPI_I2S_INT_FLAG_RBNE) == SET)
    {
        FW_SPI_RX_ISR(dev);
    }
    
    else
    {
        /**/
        return False;
    }
    
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

__INLINE_STATIC_ void SPI_IO_Init(FW_SPI_Type *dev)
{
    u32 spi = (u32)dev->SPIx;
    
    u16 mosi = dev->MOSI_Pin;
    u16 miso = dev->MISO_Pin;
    u16 sck = dev->SCK_Pin;
    u16 cs = dev->CS_Pin;
    
    FW_GPIO_Init(mosi, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
    FW_GPIO_Init(miso, FW_GPIO_Mode_IPN, FW_GPIO_Speed_High);
    FW_GPIO_Init(sck, FW_GPIO_Mode_AF_Out_PPN, FW_GPIO_Speed_High);
    FW_GPIO_Init(cs, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_High);
    
    if(spi == SPI0)
    {
        if(cs == PA4 || sck == PA5 || miso == PA6 || mosi == PA7){}
        else if(cs == PA15 || sck == PB3 || miso == PB4 || mosi == PB5)
        {
            gpio_pin_remap_config(GPIO_SPI0_REMAP, ENABLE);
        }
        else{}
    }
    else if(spi == SPI1)
    {
        if(cs == PB12 || sck == PB13 || miso == PB14 || mosi == PB15){}
    }
    else if(spi == SPI2)
    {
        if(cs == PA15 || sck == PB3 || miso == PB4 || mosi == PB5){}
        else if(cs == PA4 || sck == PC10 || miso == PC11 || mosi == PC12)
        {
            gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);  //CL型号中才有SPI2重映射
        }
    }
    else
    {
        while(1);                                            //IO配置错误
    }
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


__INLINE_STATIC_ u32 SPI_Clock_GetDIV(FW_SPI_Type *dev)
{
    u32 baudrate = dev->Baudrate;
    u32 spi = (u32)dev->SPIx;
    u32 clock = SPIx_CLK(spi);
    
    if(baudrate >= clock / 2)  return SPI_PSC_2;
    if(baudrate >= clock / 4)  return SPI_PSC_4;
    if(baudrate >= clock / 8)  return SPI_PSC_8;
    if(baudrate >= clock / 16)  return SPI_PSC_16;
    if(baudrate >= clock / 32)  return SPI_PSC_32;
    if(baudrate >= clock / 64)  return SPI_PSC_64;
    if(baudrate >= clock / 128)  return SPI_PSC_128;
    if(baudrate >= clock / 256)  return SPI_PSC_256;
    
    return SPI_PSC_256;
}


#define RCU_DMAx(dma)(\
(dma == DMA0) ? RCU_DMA0 :\
(dma == DMA1) ? RCU_DMA1 :\
(rcu_periph_enum)INVALUE)

//#define RCU_DMAx(spi)(\
//(spi == SPI0) ? RCU_DMA0 :\
//(spi == SPI1) ? RCU_DMA0 :\
//(spi == SPI2) ? RCU_DMA1 :\
//(rcu_periph_enum)INVALUE)

#define DMAx(spi)(\
(spi == SPI0) ? DMA0 :\
(spi == SPI1) ? DMA0 :\
(spi == SPI2) ? DMA1 :\
INVALUE)

#define DMA_CHx(spi, tr)(\
(spi == SPI0 && tr == TOR_RX) ? DMA_CH1 :\
(spi == SPI0 && tr == TOR_TX) ? DMA_CH2 :\
(spi == SPI1 && tr == TOR_RX) ? DMA_CH3 :\
(spi == SPI1 && tr == TOR_TX) ? DMA_CH4 :\
(spi == SPI2 && tr == TOR_RX) ? DMA_CH0 :\
(spi == SPI2 && tr == TOR_TX) ? DMA_CH1 :\
(dma_channel_enum)INVALUE)

#define SPIx_IRQn(spi)(\
(spi == SPI0) ? SPI0_IRQn :\
(spi == SPI1) ? SPI1_IRQn :\
(spi == SPI2) ? SPI2_IRQn :\
(IRQn_Type)INVALUE)

#define DMA_IRQn(spi, tr)(\
(spi == SPI0 && tr == TOR_RX) ? DMA0_Channel1_IRQn :\
(spi == SPI0 && tr == TOR_TX) ? DMA0_Channel2_IRQn :\
(spi == SPI1 && tr == TOR_RX) ? DMA0_Channel3_IRQn :\
(spi == SPI1 && tr == TOR_TX) ? DMA0_Channel4_IRQn :\
(spi == SPI2 && tr == TOR_RX) ? DMA1_Channel0_IRQn :\
(spi == SPI2 && tr == TOR_TX) ? DMA1_Channel1_IRQn :\
(IRQn_Type)INVALUE)


__INLINE_STATIC_ void SPI_DeInit(FW_SPI_Type *dev)
{
    char *name = FW_Device_GetName(dev);
    u32 spi = SPIx(name);
    
    FW_SPI_SetPort(dev, (void *)spi);
    
    spi_i2s_deinit(spi);
}

__INLINE_STATIC_ void SPI_Init(FW_SPI_Type *dev)
{
    spi_parameter_struct spi_init_struct;
    dma_parameter_struct dma_init_struct;
    
    char *name = FW_Device_GetName(dev);
    u32 spi = SPIx(name);
    
    FW_SPI_SetPort(dev, (void *)spi);
    
    u32 first_bit = (dev->First_Bit) ? SPI_ENDIAN_MSB : SPI_ENDIAN_LSB;
    u32 clock_polarity_phase;
    u32 clock_div = SPI_Clock_GetDIV(dev);    

    u32 dma = DMAx(spi);
    dma_channel_enum dma_tx_ch = DMA_CHx(spi, TOR_TX);
    dma_channel_enum dma_rx_ch = DMA_CHx(spi, TOR_RX);
    
    u8 trm;
    
    if(dev->Clock_Polarity == FW_SPI_ClockPolarity_L &&
       dev->Clock_Phase == FW_SPI_ClockPhase_Edge1)
    {
        clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    }
    else if(dev->Clock_Polarity == FW_SPI_ClockPolarity_H && 
            dev->Clock_Phase == FW_SPI_ClockPhase_Edge1)
    {
        clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    }
    else if(dev->Clock_Polarity == FW_SPI_ClockPolarity_L && 
            dev->Clock_Phase == FW_SPI_ClockPhase_Edge2)
    {
        clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    }
    else if(dev->Clock_Polarity == FW_SPI_ClockPolarity_H && 
            dev->Clock_Phase == FW_SPI_ClockPhase_Edge2)
    {
        clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    }
    else{}
        
    rcu_periph_clock_enable(RCU_SPIx(spi));
        
    SPI_IO_Init(dev);
    
    trm = FW_SPI_GetTRM(dev, TOR_TX);
    if(trm == TRM_DMA)
    {
        rcu_periph_clock_enable(RCU_DMAx(dma));
        
        dma_deinit(dma, dma_tx_ch);
        dma_init_struct.periph_addr  = (u32)&SPI_DATA(spi);
        dma_init_struct.memory_addr  = (u32)FW_SPI_GetDMABBase(dev, TOR_TX);
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_LOW;
        dma_init_struct.number       = FW_SPI_GetDMABSize(dev, TOR_TX);
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init(dma, dma_tx_ch, &dma_init_struct);
        
        dma_circulation_disable(dma, dma_tx_ch);
        dma_memory_to_memory_disable(dma, dma_tx_ch);
        
        dma_channel_disable(dma, dma_tx_ch);
        
        spi_dma_disable(spi, SPI_DMA_TRANSMIT);
    }
    else if(trm == TRM_INT)
    {
        nvic_irq_enable(SPIx_IRQn(spi), 3, 3);
        spi_i2s_interrupt_disable(spi, SPI_I2S_INT_FLAG_TBE);
    }
    else
    {
    
    }
    
    trm = FW_SPI_GetTRM(dev, TOR_RX);
    if(trm == TRM_DMA)
    {
        rcu_periph_clock_enable(RCU_DMAx(dma));
        
        dma_deinit(dma, dma_rx_ch);
        dma_init_struct.periph_addr  = (u32)&SPI_DATA(spi);
        dma_init_struct.memory_addr  = (u32)FW_SPI_GetDMABBase(dev, TOR_RX);
        dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_MEDIUM;
        dma_init_struct.number       = FW_SPI_GetDMABSize(dev, TOR_RX);
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init(dma, dma_rx_ch, &dma_init_struct);
        
        dma_circulation_disable(dma, dma_rx_ch);
        dma_memory_to_memory_disable(dma, dma_rx_ch);
        
        dma_channel_disable(dma, dma_rx_ch);
        
        spi_dma_disable(spi, SPI_DMA_RECEIVE);
    }
    else if(trm == TRM_INT)
    {
        nvic_irq_enable(SPIx_IRQn(spi), 3, 3);
        spi_i2s_interrupt_disable(spi, SPI_I2S_INT_FLAG_RBNE);
    }
    else
    {
    
    }
    
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = clock_polarity_phase;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = clock_div;
    spi_init_struct.endian               = first_bit;
    spi_init(spi, &spi_init_struct);
    
    /* set crc polynomial */
    spi_crc_polynomial_set(spi, 7);
    
    /* 默认关闭SPI */
    spi_enable(spi);
}

__INLINE_STATIC_ void SPI_CTL(FW_SPI_Type *dev, u8 state)
{
    u32 spi = (u32)dev->SPIx;
    state ? spi_enable(spi) : spi_disable(spi); 
}

__INLINE_STATIC_ void SPI_TX_CTL(FW_SPI_Type *dev, u8 state)
{
    u32 spi = (u32)dev->SPIx;
    u8 trm = FW_SPI_GetTRM(dev, TOR_TX);
    
    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(spi);
        dma_channel_enum dma_tx_ch = DMA_CHx(spi, TOR_TX);
        
        if(state)
        {
            dma_channel_enable(dma, dma_tx_ch);
        }
        else
        {
            dma_channel_disable(dma, dma_tx_ch);
        }
    }
    else if(trm == TRM_INT)
    {
        if(state)
            spi_i2s_interrupt_enable(spi, SPI_I2S_INT_FLAG_TBE);
        else
            spi_i2s_interrupt_disable(spi, SPI_I2S_INT_FLAG_TBE);
    }
    else
    {
        SPI_CTL(dev, state);
    }
}

__INLINE_STATIC_ void SPI_RX_CTL(FW_SPI_Type *dev, u8 state)
{
    u32 spi = (u32)dev->SPIx;
    u8 trm = FW_SPI_GetTRM(dev, TOR_RX);
    
    if(trm == TRM_DMA)
    {
        u32 dma = DMAx(spi);
        dma_channel_enum dma_rx_ch = DMA_CHx(spi, TOR_RX);
        
        if(state)
        {
            dma_channel_enable(dma, dma_rx_ch);
        }
        else
        {
            dma_channel_disable(dma, dma_rx_ch);
        }
    }
    else if(trm == TRM_INT)
    {
        if(state)
            spi_i2s_interrupt_enable(spi, SPI_I2S_INT_FLAG_RBNE);
        else
            spi_i2s_interrupt_disable(spi, SPI_I2S_INT_FLAG_RBNE);
    }
    else
    {
        SPI_CTL(dev, state);
    }
}

__INLINE_STATIC_ void SPI_TX_Byte(FW_SPI_Type *dev, u8 value)
{
    spi_i2s_data_transmit((u32)dev->SPIx, value);
}

__INLINE_STATIC_ u8   SPI_RX_Byte(FW_SPI_Type *dev)
{
    return spi_i2s_data_receive((u32)dev->SPIx);
}

__INLINE_STATIC_ u8   SPI_Wait_TC(FW_SPI_Type *dev)
{
    u32 spi = (u32)dev->SPIx;
    return (u8)spi_i2s_flag_get(spi, SPI_FLAG_TBE);
}

__INLINE_STATIC_ u8   SPI_Wait_RC(FW_SPI_Type *dev)
{
    u32 spi = (u32)dev->SPIx;
    return (u8)spi_i2s_flag_get(spi, SPI_FLAG_RBNE);
}

__INLINE_STATIC_ u32  SPI_Write(FW_SPI_Type *dev, const u8 *pdata, u32 num)
{
    u32 spi = (u32)dev->SPIx;
    u32 dma = DMAx(spi);
    dma_channel_enum dma_tx_ch = DMA_CHx(spi, TOR_TX);
    
    dma_channel_disable(dma, dma_tx_ch);
    
    dma_flag_clear(dma, dma_tx_ch, DMA_FLAG_FTF);
    
    dma_memory_address_config(dma, dma_tx_ch, (u32)pdata);
    dma_transfer_number_config(dma, dma_tx_ch, num);
    dma_memory_increase_enable(dma, dma_tx_ch);
    
    dma_channel_enable(dma, dma_tx_ch);
    spi_dma_enable(spi, SPI_DMA_TRANSMIT);
    while(dma_flag_get(dma, dma_tx_ch, DMA_FLAG_FTF) != SET);
    spi_dma_disable(spi, SPI_DMA_TRANSMIT);
}

__INLINE_STATIC_ u32  SPI_Read(FW_SPI_Type *dev, u8 *pdata, u32 num)
{
    u32 spi = (u32)dev->SPIx;
    u32 dma = DMAx(spi);
    dma_channel_enum dma_tx_ch = DMA_CHx(spi, TOR_TX);
    dma_channel_enum dma_rx_ch = DMA_CHx(spi, TOR_RX);
    
    dma_channel_disable(dma, dma_tx_ch);
    dma_channel_disable(dma, dma_rx_ch);
    
    dma_flag_clear(dma, dma_rx_ch, DMA_FLAG_FTF);
    
    dma_memory_address_config(dma, dma_tx_ch, (u32)pdata);
    dma_transfer_number_config(dma, dma_tx_ch, num);
    
    dma_memory_address_config(dma, dma_rx_ch, (u32)pdata);
    dma_transfer_number_config(dma, dma_rx_ch, num);
    
    spi_i2s_data_receive(spi);
    
    dma_channel_enable(dma, dma_tx_ch);
    dma_channel_enable(dma, dma_rx_ch);
    spi_dma_enable(spi, SPI_DMA_RECEIVE);
    spi_dma_enable(spi, SPI_DMA_TRANSMIT);
    while(dma_flag_get(dma, dma_rx_ch, DMA_FLAG_FTF) != SET);
    spi_dma_disable(spi, SPI_DMA_TRANSMIT);
    spi_dma_disable(spi, SPI_DMA_RECEIVE);
}



/* Native SPI Driver */
__CONST_STATIC_ FW_SPI_Driver_Type HSPI_Driver =
{
    .DeInit  = SPI_DeInit,
    .Init    = SPI_Init,
    
    .CTL     = SPI_CTL,
    .TX_CTL  = SPI_TX_CTL,
    .RX_CTL  = SPI_RX_CTL,
    
    .TX_Byte = SPI_TX_Byte,
    .RX_Byte = SPI_RX_Byte,
    
    .Wait_TC = SPI_Wait_TC,
    .Wait_RC = SPI_Wait_RC,
    
    .Write   = SPI_Write,
    .Read    = SPI_Read,
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

