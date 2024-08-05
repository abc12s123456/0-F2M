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
#ifndef _FW_SPI_H_
#define _FW_SPI_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "serial.h"


/* 起始位 */
typedef enum
{
    FW_SPI_FirstBit_LSB = 0,
    FW_SPI_FirstBit_MSB = 1,
}FW_SPI_FirstBit_Enum;


/* 同步时钟的空闲状态 */
typedef enum
{
    FW_SPI_ClockPolarity_L,
    FW_SPI_ClockPolarity_H,
}FW_SPI_ClockPolarity_Enum;


/* 同步时钟相位，决定在同步时钟的第几个跳变沿采样数据*/
typedef enum
{
    FW_SPI_ClockPhase_Edge1,
    FW_SPI_ClockPhase_Edge2,
}FW_SPI_ClockPhase_Enum;


/* 数据总线位宽 */
typedef enum
{
    FW_SPI_DataWidth_4Bits = 4,
    FW_SPI_DataWidth_5Bits,
    FW_SPI_DataWidth_6Bits,
    FW_SPI_DataWidth_7Bits,
    FW_SPI_DataWidth_8Bits,
    FW_SPI_DataWidth_9Bits,
    FW_SPI_DataWidth_10Bits,
    FW_SPI_DataWidth_11Bits,
    FW_SPI_DataWidth_12Bits,
    FW_SPI_DataWidth_13Bits,
    FW_SPI_DataWidth_14Bits,
    FW_SPI_DataWidth_15Bits,
    FW_SPI_DataWidth_16Bits,
}FW_SPI_DataWidth_Enum;


typedef struct FW_SPI   FW_SPI_Type;

/* SPI Device */
struct FW_SPI
{
    FW_Device_Type Device;
    
    SGC_Type Config;
    
    /* IO */
    u16 MOSI_Pin;
    u16 MISO_Pin;
    u16 SCK_Pin;
    u16 CS_Pin;
    
    /* FIFO Buffet Size */
    u16 TX_FIFO_Buffer_Size;
    u16 RX_FIFO_Buffer_Size;
    
    /* DMA Buffer Size */
    u16 TX_DMA_Buffer_Size;
    u16 RX_DMA_Buffer_Size;
    
    /* Baudrate */
    u32 Baudrate;
    
    /*  */
    u32 Data_Width : 5;
    u32 Clock_Polarity : 1;
    u32 Clock_Phase : 1;
    u32 First_Bit : 1;
    
    /* CS Valid Level */
    u32 CS_VL : 1;
    
    /* 模拟SPI的延时时间，单位：us */
    u32 SDTime : 16;
    
    u32 : 7;
    
    __RO_ void *SPIx;
    
    /* CS控制 */
    __RO_ void (*CS_Set)(FW_SPI_Type *dev, u8 state);

    /* IO模拟SPI实现的接口 */
    __RO_ u8   (*TR_Byte)(FW_SPI_Type *dev, u8 value);
    __RO_ void (*SCK_Idle)(FW_SPI_Type *dev);
    __RO_ void (*SCK_Edge)(FW_SPI_Type *dev);
};


/* Driver */
typedef struct FW_SPI_Driver
{
    void (*DeInit)(FW_SPI_Type *dev);
    void (*Init)(FW_SPI_Type *dev);
    
    void (*CTL)(FW_SPI_Type *dev, u8 state);
    void (*TX_CTL)(FW_SPI_Type *dev, u8 state);
    void (*RX_CTL)(FW_SPI_Type *dev, u8 state);
    
    void (*TX_Byte)(FW_SPI_Type *dev, u8 value);
    u8   (*RX_Byte)(FW_SPI_Type *dev);
    
    void (*TX_Short)(FW_SPI_Type *dev, u16 value);
    u16  (*RX_Short)(FW_SPI_Type *dev);
    
    u8   (*Wait_TC)(FW_SPI_Type *dev);
    u8   (*Wait_RC)(FW_SPI_Type *dev);
    
    void (*Set_TDL)(FW_SPI_Type *dev, u32 length);
    u32  (*Get_RDL)(FW_SPI_Type *dev);
    
    void (*CS_Set)(FW_SPI_Type *dev, u8 state);
    
    u32  (*Write)(FW_SPI_Type *dev, const u8 *pdata, u32 num);
    u32  (*Read)(FW_SPI_Type *dev, u8 *pdata, u32 num);
}FW_SPI_Driver_Type;


void FW_SPI_SetPort(FW_SPI_Type *dev, void *spi);
void *FW_SPI_GetPort(FW_SPI_Type *dev);

u8   FW_SPI_GetTRM(FW_SPI_Type *dev, u8 tr);
void *FW_SPI_GetDMABBase(FW_SPI_Type *dev, u8 tr);
u32  FW_SPI_GetDMABSize(FW_SPI_Type *dev, u8 tr);

void FW_SPI_DeInit(FW_SPI_Type *dev);
void FW_SPI_Init(FW_SPI_Type *dev);

void FW_SPI_CTL(FW_SPI_Type *dev, u8 state);
void FW_SPI_TXCTL(FW_SPI_Type *dev, u8 state);
void FW_SPI_RXCTL(FW_SPI_Type *dev, u8 state);

u32  FW_SPI_Write(FW_SPI_Type *dev, u32 offset, const u8 *pdata, u32 num);
u32  FW_SPI_Read(FW_SPI_Type *dev, u32 offset, u8 *pdata, u32 num);

void FW_SPI_CSSet(FW_SPI_Type *dev, u8 state);

void FW_SPI_RX_ISR(FW_SPI_Type *dev);
void FW_SPI_RC_ISR(FW_SPI_Type *dev);
void FW_SPI_TX_ISR(FW_SPI_Type *dev);
void FW_SPI_TC_ISR(FW_SPI_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

