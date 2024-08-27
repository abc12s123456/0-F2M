/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
#include "fw_spi.h"


#if defined (SSPI_MOD_EN) && SSPI_MOD_EN


#include "fw_gpio.h"
#include "fw_delay.h"

#include "libc.h"


__INLINE_STATIC_ void SCK_CPOL0(FW_SPI_Type *dev)
{
    FW_GPIO_CLR(dev->SCK_Pin);
}

__INLINE_STATIC_ void SCK_CPOL1(FW_SPI_Type *dev)
{
    FW_GPIO_SET(dev->SCK_Pin);
}

__INLINE_STATIC_ void SCK_RE(FW_SPI_Type *dev)
{
    FW_GPIO_CLR(dev->SCK_Pin);
    FW_Delay_Us(dev->SDTime);
    FW_GPIO_SET(dev->SCK_Pin);
}

__INLINE_STATIC_ void SCK_FE(FW_SPI_Type *dev)
{
    FW_GPIO_SET(dev->SCK_Pin);
    FW_Delay_Us(dev->SDTime);
    FW_GPIO_CLR(dev->SCK_Pin);
}

__INLINE_STATIC_ u8   TRByte_MSB(FW_SPI_Type *dev, u8 value)
{
    u8 i, tmp = 0;
    
    for(i = 0; i < 8; i++)
    {
        /* 写 */
        if(value & 0x80)  FW_GPIO_SET(dev->MOSI_Pin);
        else  FW_GPIO_CLR(dev->MOSI_Pin);
        value <<= 1;
        dev->SCK_Edge(dev);
        
        /* 读 */
        tmp <<= 1;
        if(FW_GPIO_Read(dev->MISO_Pin) == LEVEL_H)  tmp |= 1;
    }
    
    /* SCK恢复默认，等待下一次操作 */
    dev->SCK_Idle(dev);
    
    /* 置位主机输出，降低功耗 */
    FW_GPIO_SET(dev->MOSI_Pin);
    
    return tmp;
}

__INLINE_STATIC_ u8   TRByte_LSB(FW_SPI_Type *dev, u8 value)
{
    u8 i, tmp = 0;
    
    for(i = 0; i < 8; i++)
    {
        /* 写 */
        if(value & 0x01)  FW_GPIO_SET(dev->MOSI_Pin);
        else  FW_GPIO_CLR(dev->MOSI_Pin);
        value >>= 1;
        dev->SCK_Edge(dev);
        
        /* 读 */
        tmp <<= i;
        if(FW_GPIO_Read(dev->MISO_Pin) == LEVEL_H)  tmp |= 1;
    }
    
    /* SCK恢复默认，等待下一次操作 */
    dev->SCK_Idle(dev);
    
    /* 置位主机输出，降低功耗 */
    FW_GPIO_SET(dev->MOSI_Pin);
    
    return tmp;
}

__INLINE_STATIC_ void SSPI_DeInit(FW_SPI_Type *dev)
{
    memset(dev, 0, sizeof(FW_SPI_Type));
}

__INLINE_STATIC_ void SSPI_Init(FW_SPI_Type *dev)
{
    FW_GPIO_Init(dev->MISO_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->MOSI_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    if(dev->First_Bit == FW_SPI_FirstBit_MSB)
    {
        dev->TR_Byte = TRByte_MSB;
    }
    else
    {
        dev->TR_Byte = TRByte_LSB;
    }
    
    if(dev->Clock_Polarity == FW_SPI_ClockPolarity_H)
    {
        FW_GPIO_Init(dev->SCK_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        FW_GPIO_SET(dev->SCK_Pin);
        
        dev->SCK_Idle = SCK_CPOL1;
        
        if(dev->Clock_Phase == FW_SPI_ClockPhase_Edge1)
        {
            dev->SCK_Edge = SCK_FE;
        }
        else
        {
            dev->SCK_Edge = SCK_RE;
        }
    }
    else
    {
        FW_GPIO_Init(dev->SCK_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
        FW_GPIO_CLR(dev->SCK_Pin);
        
        dev->SCK_Idle = SCK_CPOL0;
        
        if(dev->Clock_Phase == FW_SPI_ClockPhase_Edge1)
        {
            dev->SCK_Edge = SCK_RE;
        }
        else
        {
            dev->SCK_Edge = SCK_FE;
        }
    }
    
    if(dev->CS_VL == LEVEL_L)
    {
        FW_GPIO_Init(dev->CS_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        FW_GPIO_SET(dev->CS_Pin);
    }
    else
    {
        FW_GPIO_Init(dev->CS_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
        FW_GPIO_CLR(dev->CS_Pin);
    }
    
    /* 置位输出，降低功耗 */
    FW_GPIO_SET(dev->MOSI_Pin);
    
    if(dev->Baudrate >= 1000000)
    {
        dev->SDTime = 1;
    }
    else
    {
        dev->SDTime = 1000000 / dev->Baudrate;
    }
}

__INLINE_STATIC_ u32 SSPI_Write(FW_SPI_Type *dev, const u8 *pdata, u32 num)
{
    u32 i;
    for(i = 0; i < num; i++)
    {
        dev->TR_Byte(dev, *pdata++);
    }
    return num;
}

__INLINE_STATIC_ u32 SSPI_Read(FW_SPI_Type *dev, u8 *pdata, u32 num)
{
    u32 i;
    for(i = 0; i < num; i++)
    {
        *pdata++ = dev->TR_Byte(dev, 0xFF);
    }
    return num;
}

__INLINE_STATIC_ void SSPI_CS_Set(FW_SPI_Type *dev, u8 state)
{
    if(state)
        FW_GPIO_Write(dev->CS_Pin, dev->CS_VL);
    else
        FW_GPIO_Write(dev->CS_Pin, !dev->CS_VL);
}




/* IO Simular SPI */
__CONST_STATIC_ FW_SPI_Driver_Type SSPI_Driver =
{
    .DeInit = SSPI_DeInit,
    .Init = SSPI_Init,
    
    .Write = SSPI_Write,
    .Read = SSPI_Read,
    
    .CS_Set = SSPI_CS_Set,
};
FW_DRIVER_REGIST("io->spi", &SSPI_Driver, SSPI);


#endif  /* defined (SSPI_MOD_EN) && SSPI_MOD_EN */
