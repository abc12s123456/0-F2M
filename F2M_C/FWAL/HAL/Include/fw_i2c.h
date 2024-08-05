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
#ifndef _FW_SI2C_H_
#define _FW_SI2C_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "serial.h"


/**
 * I2C驱动编号
 * 外部扩展可以有多个不同的驱动
 */
#define I2C_DRV_NATIVE_NUM        0x00      //原生
#define I2C_DRV_IOSIM_NUM         0x01      //IO模拟
#define I2C_DRV_EXT_NUM           0x02      //扩展


/* 起始操作位 */
typedef enum
{
    FW_I2C_FirstBit_LSB = 0,
    FW_I2C_FirstBit_MSB = 1,
}FW_I2C_FirstBit_Enum;


/* 设备地址格式 */
typedef enum
{
    FW_I2C_DeviceAddr_7Bits = 0,
    FW_I2C_DeviceAddr_10Bits = 1,
}FW_I2C_DeviceAddr_Enum;



typedef struct FW_I2C   FW_I2C_Type;


struct FW_I2C
{
    FW_Device_Type Device;
    
    SGC_Type Config;
    
    /* IO */
    u16  SCL_Pin;
    u16  SDA_Pin;
    
    /* FIFO Buffet Size */
    u16  TX_FIFO_Buffer_Size;
    u16  RX_FIFO_Buffer_Size;
    
    /* DMA Buffer Size */
    u16  TX_DMA_Buffer_Size;
    u16  RX_DMA_Buffer_Size;
    
    /* I2C波特率，Bmax = 5Mbps */
    u32  Baudrate : 24;
    
    u32  First_Bit : 1;
    
    /* 针对某些设备的必要延时时间，单位：ms */
    u32  D0 : 7;
    u32  D1 : 7;
    u32  D2 : 7;
    u32  D3 : 7;
    
    /* I2C设备ID(即设备地址) */
    u32  ID : 10;
    
    /* 地址模式 */
    u32  Addr_Mode : 1;
    
    void *I2Cx;
};


typedef struct
{
    void (*DeInit)(FW_I2C_Type *dev);
    void (*Init)(FW_I2C_Type *dev);
    
    void (*CTL)(FW_I2C_Type *dev, u8 state);
    
//    void (*TX_Byte)(FW_I2C_Type *dev, u8 value);
//    u8   (*RX_Byte)(FW_I2C_Type *dev);
//    
//    u8   (*Wait_TC)(FW_I2C_Type *dev);
//    u8   (*Wait_RC)(FW_I2C_Type *dev);
//    
//    void (*Set_TDL)(FW_I2C_Type *dev, u32 length);
//    u32  (*Get_RDL)(FW_I2C_Type *dev);
    
    u32  (*Write)(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num);
    u32  (*Read)(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num);
    
    void (*Set_Delay)(FW_I2C_Type *dev, u8 d0, u8 d1, u8 d2, u8 d3);
}FW_I2C_Driver_Type;


/*  */
__INLINE_STATIC_ u32  FW_I2C_GetAccAddr(u32 addr, u32 num)
{
    return ((num << 24) | addr);
}

void FW_I2C_SetPort(FW_I2C_Type *dev, void *i2c);
void *FW_I2C_GetPort(FW_I2C_Type *dev);
u8   FW_I2C_GetTRM(FW_I2C_Type *dev, u8 tr);
void *FW_I2C_GetDMABBase(FW_I2C_Type *dev, u8 tr);
u32  FW_I2C_GetDMABSize(FW_I2C_Type *dev, u8 tr);

void FW_I2C_DeInit(FW_I2C_Type *dev);
void FW_I2C_Init(FW_I2C_Type *dev);

void FW_I2C_CTL(FW_I2C_Type *dev, u8 state);

u32  FW_I2C_Write(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num);
u32  FW_I2C_Read(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num);

void FW_I2C_SetDelay(FW_I2C_Type *dev, u8 d0, u8 d1, u8 d2, u8 d3);


#ifdef __cplusplus
}
#endif

#endif

