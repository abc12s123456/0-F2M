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
#include "fw_i2c.h"
#include "fw_gpio.h"
#include "fw_delay.h"

#include "libc.h"


__INLINE_STATIC_ void SCL_OUT(FW_I2C_Type *dev, u8 level)
{
    if(level)
        FW_GPIO_SET(dev->SCL_Pin);
    else
        FW_GPIO_CLR(dev->SCL_Pin);
}

__INLINE_STATIC_ void SDA_OUT(FW_I2C_Type *dev, u8 level)
{
    
    if(level)
        FW_GPIO_SET(dev->SDA_Pin);
    else
        FW_GPIO_CLR(dev->SDA_Pin);
}

__INLINE_STATIC_ u8   SDA_IN(FW_I2C_Type *dev)
{
    return FW_GPIO_Read(dev->SDA_Pin);
}


/**
@功能：起始信号
@参数：Type, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_Start(FW_I2C_Type *dev)
{
    SCL_OUT(dev, LEVEL_H);
    
    SDA_OUT(dev, LEVEL_H);
    FW_Delay_Us(dev->SDTime);
    SDA_OUT(dev, LEVEL_L);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_L);
}
/**/

/**
@功能：停止信号
@参数：dev, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_Stop(FW_I2C_Type *dev)
{
    SCL_OUT(dev, LEVEL_H);
    
    SDA_OUT(dev, LEVEL_L);
    FW_Delay_Us(dev->SDTime);
    SDA_OUT(dev, LEVEL_H);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_L);
}
/**/

/**
@功能：发送一个字节
@参数：dev, I2C端口对象
       value, 发送的数据
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_Write_Byte(FW_I2C_Type *dev, u8 value)
{
    SCL_OUT(dev, LEVEL_L);
    
    if(dev->First_Bit)
    {
        for(u8 i = 0; i < 8; i++)
        {
            if((value & 0x80) == 0x80)
            {
                SDA_OUT(dev, LEVEL_H);           //MSB开始发送
            }
            else
            {
                SDA_OUT(dev, LEVEL_L);
            }    
            value <<= 1;                         //下一个bit
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(dev->SDTime);
            SCL_OUT(dev, LEVEL_L);
            FW_Delay_Us(dev->SDTime);
        }
    }
    else
    {
        for(u8 i = 0; i < 8; i++)
        {
            if((value & 0x01) == 0x01)
            {
                SDA_OUT(dev, LEVEL_H);           //LSB开始发送
            }
            else
            {
                SDA_OUT(dev, LEVEL_L);
            }    
            value >>= 1;                         //下一个bit
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(dev->SDTime);
            SCL_OUT(dev, LEVEL_L);
            FW_Delay_Us(dev->SDTime);
        }
    }
}
/**/

/**
@功能：读取一个字节
@参数：dev, I2C端口对象
@返回：读取到的数据
@说明：
*/
__INLINE_STATIC_ u8 SI2C_Read_Byte(FW_I2C_Type *dev)
{
    u8 value = 0;
    
    SDA_OUT(dev, LEVEL_H);
    
    FW_GPIO_Init(dev->SDA_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
    SDA_IN(dev);                                 //空读
    if(dev->First_Bit)
    {
        for(u8 i = 0; i < 8; i++)
        {
            SCL_OUT(dev, LEVEL_L);
            FW_Delay_Us(dev->SDTime);
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(dev->SDTime);
            value <<= 1;                         //MSB开始读取
            value |= SDA_IN(dev);
        }
    }
    else
    {
        for(u8 i = 0; i < 8; i++)
        {
            SCL_OUT(dev, LEVEL_L);
            FW_Delay_Us(dev->SDTime);
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(dev->SDTime);
            value <<= i;                          //LSB开始读取
            value |= SDA_IN(dev);
        }
    }
    
    FW_GPIO_Init(dev->SDA_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_High);
    SCL_OUT(dev, LEVEL_L);
    return value;
}
/**/

/**
@功能：非应答信号
@参数：dev, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_NAK(FW_I2C_Type *dev)
{
    SDA_OUT(dev, LEVEL_H);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_H);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_L);
    FW_Delay_Us(dev->SDTime);
}

/**
@功能：应答信号
@参数：dev, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_ACK(FW_I2C_Type *dev)
{
    SDA_OUT(dev, LEVEL_L);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_H);
    FW_Delay_Us(dev->SDTime);
    
    SCL_OUT(dev, LEVEL_L);
    FW_Delay_Us(dev->SDTime);
}
/**/

/**
@功能：模拟I2C初始化
@参数：Type, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void FW_SI2C_Init(FW_I2C_Type *dev)
{
    FW_GPIO_Init(dev->SCL_Pin, FW_GPIO_Mode_Out_ODN, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->SDA_Pin, FW_GPIO_Mode_Out_ODN, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(dev->SCL_Pin);
    FW_GPIO_SET(dev->SDA_Pin);
    
    /* 模拟I2C通信速率最大100Kbps */
    if(dev->Baudrate >= 1000000)
    {
        dev->SDTime = 1;
    }
    else
    {
        dev->SDTime = 1000000 / dev->Baudrate;
    }
}
/**/

/* 模拟I2C中，addr的高8位保存有效地址的位数 */
__INLINE_STATIC_ u32  FW_SI2C_Write(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num)
{
    u32 i;
    u8 addr_num;
    u8 addr_value;
    
    SI2C_Start(dev);
    
    if(dev->Device_Addr_Mode == FW_I2C_DAM_10Bits)
    {
        u8 addr_h, addr_l;
        addr_h = FW_I2C_Get10BAddrH(dev->ID);
        addr_l = FW_I2C_Get10BAddrL(dev->ID);
        
        SI2C_Write_Byte(dev, addr_h);
        SI2C_ACK(dev);
        
        SI2C_Write_Byte(dev, addr_l);
        SI2C_ACK(dev);
    }
    else
    {
        SI2C_Write_Byte(dev, dev->ID);
        SI2C_ACK(dev);
    }
    
    /* addr = addr_num[31 : 24] + addr_h[23 : 16] + addr_m[15 : 8] + addr_l[7 : 0] */
//    addr_num = (u8)((addr >> 24) + 1);
    addr_num = dev->Access_Addr_Mode + 1;
    for(i = 0; i < addr_num; i++)
    {
        addr_value = (u8)((addr >> (i << 3)) & 0xFF);
        SI2C_Write_Byte(dev, addr_value);
        SI2C_ACK(dev);
    }
    
    for(i = 0; i < num; i++)
    {
        SI2C_Write_Byte(dev, *pdata++);
//        SI2C_ACK(dev);
    }
    
    SI2C_Stop(dev);
    
    return num;
}

__INLINE_STATIC_ u32  FW_SI2C_Read(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num)
{
    u32 i;
    u8 addr_num;
    u8 addr_value;
    
    SI2C_Start(dev);
    
    if(dev->Device_Addr_Mode == FW_I2C_DAM_10Bits)
    {
        u8 addr_h, addr_l;
        addr_h = FW_I2C_Get10BAddrH(dev->ID);
        addr_l = FW_I2C_Get10BAddrL(dev->ID);
        
        SI2C_Write_Byte(dev, addr_h);
        SI2C_ACK(dev);
        
        SI2C_Write_Byte(dev, addr_l);
        SI2C_ACK(dev);
    }
    else
    {
        SI2C_Write_Byte(dev, dev->ID);
        SI2C_ACK(dev);
    }
    
//    addr_num = (u8)((addr >> 24) + 1);
    addr_num = dev->Access_Addr_Mode + 1;
    for(i = 0; i < addr_num; i++)
    {
        addr_value = (u8)((addr >> (i << 3)) & 0xFF);
        SI2C_Write_Byte(dev, addr_value);
        SI2C_ACK(dev);
    }
    
    SI2C_Start(dev);
    
    if(dev->Device_Addr_Mode == FW_I2C_DAM_10Bits)
    {
        u8 addr_h, addr_l;
        addr_h = FW_I2C_Get10BAddrH(dev->ID);
        addr_l = FW_I2C_Get10BAddrL(dev->ID);
        
        SI2C_Write_Byte(dev, addr_h | 0x01);
        SI2C_ACK(dev);
        
        SI2C_Write_Byte(dev, addr_l);
        SI2C_ACK(dev);
    }
    else
    {
        SI2C_Write_Byte(dev, dev->ID | 0x01);
        SI2C_ACK(dev);
    }
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = SI2C_Read_Byte(dev);
        SI2C_ACK(dev);
    }
    
    SCL_OUT(dev, LEVEL_L);
    SI2C_NAK(dev);
    
    SI2C_Stop(dev);
    
    return num;
}




/* IO Simular I2C */
__CONST_STATIC_ FW_I2C_Driver_Type SI2C_Driver =
{
    .Init = FW_SI2C_Init,
    
    .Write = FW_SI2C_Write,
    .Read = FW_SI2C_Read,
};
FW_DRIVER_REGIST("io->i2c", &SI2C_Driver, SI2C);

