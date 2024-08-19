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


#include "fw_mod_conf.h"


#if defined(I2C_MOD_EN) && I2C_MOD_EN


#include "fw_device.h"
#include "serial.h"


/* 起始操作位 */
typedef enum
{
    FW_I2C_FirstBit_LSB = 0,
    FW_I2C_FirstBit_MSB = 1,
}FW_I2C_FirstBit_Enum;


/* 设备地址格式 */
typedef enum
{
    FW_I2C_DAM_7Bits = 0,
    FW_I2C_DAM_10Bits = 1,
    FW_I2C_DAM_Dual = 2,
}FW_I2C_Device_Addr_Mode_Enum, FW_I2C_DAM_Enum;


typedef enum
{
    FW_I2C_AAM_8Bits = 0,
    FW_I2C_AAM_16Bits,
    FW_I2C_AAM_32Bits,
}FW_I2C_Access_Addr_Mode_Enum, FW_I2C_AAM_Enum;


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
    u32  Baudrate : 23;
    
    u32  First_Bit : 1;
    
    /* 设备地址模式 */
    u32  Device_Addr_Mode : 2;
    
    /* 访问地址模式 */
    u32  Access_Addr_Mode : 2;
    
    u32  : 4;
    
    /* 针对某些设备的必要延时时间，单位：ms */
    u32  D0 : 4;
    u32  D1 : 4;
    u32  D2 : 4;
    u32  D3 : 4;
    
    /* I2C设备ID(即设备地址) */
    u32  ID : 16;
    
    /* 模拟I2C的延时时间，单位：us */
    u32 SDTime : 16;
    
    void *I2Cx;
};


typedef struct
{
    void (*DeInit)(FW_I2C_Type *dev);
    void (*Init)(FW_I2C_Type *dev);
    
    void (*CTL)(FW_I2C_Type *dev, u8 state);
    void (*TX_CTL)(FW_I2C_Type *dev, u8 state);
    void (*RX_CTL)(FW_I2C_Type *dev, u8 state);
    
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


/*  */
__INLINE_STATIC_ u32  FW_I2C_GetActualAddr(u32 addr, u32 num)
{
    return ((num << 24) | addr);
}

__INLINE_STATIC_ u8   FW_I2C_Get10BAddrH(u16 addr)
{
    addr >>= 7;
    addr &= 0x06;
    addr |= 0xF0;
    return (u8)addr;
}

__INLINE_STATIC_ u8   FW_I2C_Get10BAddrL(u16 addr)
{
    return (u8)addr;
}


#endif  /* defined(I2C_MOD_EN) && I2C_MOD_EN */

#ifdef __cplusplus
}
#endif

#endif

