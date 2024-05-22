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
    FW_Delay_Us(10);
    SDA_OUT(dev, LEVEL_L);
    FW_Delay_Us(10);
    
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
    FW_Delay_Us(10);
    SDA_OUT(dev, LEVEL_H);
    FW_Delay_Us(10);
    
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
            FW_Delay_Us(10);
            SCL_OUT(dev, LEVEL_L);
//            FW_Delay_Us(10);
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
            FW_Delay_Us(10);
            SCL_OUT(dev, LEVEL_L);
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
            FW_Delay_Us(10);
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(10);
            value <<= 1;                         //MSB开始读取
            value |= SDA_IN(dev);
        }
    }
    else
    {
        for(u8 i = 0; i < 8; i++)
        {
            SCL_OUT(dev, LEVEL_L);
            FW_Delay_Us(10);
            SCL_OUT(dev, LEVEL_H);
            FW_Delay_Us(10);
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
@功能：应答信号
@参数：dev, I2C端口对象
@返回：无
@说明：
*/
__INLINE_STATIC_ void SI2C_ACK(FW_I2C_Type *dev, u8 ack)
{
    if(ack)                                      //非应答信号
    {
        SDA_OUT(dev, LEVEL_H);
        FW_Delay_Us(5);
        
        SCL_OUT(dev, LEVEL_H);
        FW_Delay_Us(10);
        
        SCL_OUT(dev, LEVEL_L);
        FW_Delay_Us(5);
    }
    else                                         //应答信号
    {
        SDA_OUT(dev, LEVEL_L);
        FW_Delay_Us(5);
        
        SCL_OUT(dev, LEVEL_H);
        FW_Delay_Us(10);
        
        SCL_OUT(dev, LEVEL_L);
        FW_Delay_Us(5);
    }
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
    FW_GPIO_Init(dev->SCL_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->SDA_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(dev->SCL_Pin);
    FW_GPIO_SET(dev->SDA_Pin);
}
/**/

///**
//@功能：模拟I2C向设备写一个字节
//@参数：Type, I2C端口对象
//       addr, 数据写入地址
//       Data, 写入的数据
//@返回：无
//@说明：
//*/
//void FW_SI2C_WriteByte(FW_I2C_Type *dev, u8 *paddr, u8 num_addr, u8 value)
//{
//    u8 i;
//    
//    SI2C_Start(dev);                                  //起始信号
//    
//    if(dev->ID)                                  //从设备地址不为0
//    {
//        SI2C_Write_Byte(dev, dev->ID);                //写入从设备地址
//        SI2C_ACK(dev, 0);                             //发送应答信号
//    }
//    
//    for(i = 0; i < num_addr; i++)
//    {
//        SI2C_Write_Byte(dev, paddr[num_addr - 1 - i]);
//        SI2C_ACK(dev, 0);
//    }
//    FW_Delay_Ms(dev->D0);
//    
////    Write_Byte(dev, addr);                      //写入数据地址
////    ACK(dev, 0);
//    
//    SI2C_Write_Byte(dev, value);                       //写数据
//    SI2C_ACK(dev, 0);
//    FW_Delay_Ms(dev->D2);
//    
//    SI2C_Stop(dev);                                    //结束信号
//    FW_Delay_Ms(dev->D3);                         //
//}
///**/

///**
//@功能：模拟I2C读取一个字节
//@参数：Type, I2C端口对象
//       addr, 读取数据的地址
//@返回：读取到的数据
//@说明：
//*/
//u8 FW_SI2C_ReadByte(FW_I2C_Type *dev, u8 *paddr, u8 num_addr)
//{
//    u8 value = 0;
//    u8 i;
//    
//    SI2C_Start(dev);
//    
//    if(dev->ID)
//    {
//        SI2C_Write_Byte(dev, dev->ID);
//        SI2C_ACK(dev, 0);
//    }
//    
//    for(i = 0; i < num_addr; i++)
//    {
//        SI2C_Write_Byte(dev, paddr[num_addr - 1 - i]);
//        SI2C_ACK(dev, 0);
//    }
//    FW_Delay_Ms(dev->D0);
//    
//    SI2C_Start(dev);                                  //重启，进行读操作
//    if(dev->ID)
//    {
//        SI2C_Write_Byte(dev, dev->ID + 1);            //
//        SI2C_ACK(dev, 0);
//    }
//    FW_Delay_Ms(dev->D1);
//    
//    value = SI2C_Read_Byte(dev);                      //读数据
//    FW_Delay_Ms(dev->D2);
//    
//    SCL_OUT(dev, LEVEL_L);                       //SLC拉低，发送非应答，结束操作
//    SI2C_ACK(dev, 1);
//    SI2C_Stop(dev);
//    FW_Delay_Ms(dev->D3);
//    
//    return value;
//}
///**/

//__INLINE_STATIC_ void FW_SI2C_SetDelay(FW_I2C_Type *dev, u8 d0, u8 d1, u8 d2, u8 d3)
//{
//    dev->D0 = d0;
//    dev->D1 = d1;
//    dev->D2 = d2;
//    dev->D3 = d3;
//}
///**/

#if 1
/* 模拟I2C中，addr的高8位保存有效地址的位数 */
__INLINE_STATIC_ u32  FW_SI2C_Write(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num)
{
    #if 0
    u32 i;
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID);
    SI2C_ACK(dev, 0);
    
    SI2C_Write_Byte(dev, addr);
    SI2C_ACK(dev, 0);
    
    for(i = 0; i < num; i++)
    {
        SI2C_Write_Byte(dev, *pdata++);
        SI2C_ACK(dev, 0);
    }
    
    SI2C_Stop(dev);
    
    FW_Delay_Ms(5);
    #else
    u32 i;
    u8 addr_num;
    u8 addr_value;
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID);
    SI2C_ACK(dev, 0);
    FW_Delay_Ms(dev->D0);
    
    /* addr = addr_num[31 : 24] + addr_h[23 : 16] + addr_m[15 : 8] + addr_l[7 : 0] */
    addr_num = (u8)((addr >> 24) + 1);
    for(i = 0; i < addr_num; i++)
    {
        addr_value = (u8)((addr >> (i << 3)) & 0xFF);
        SI2C_Write_Byte(dev, addr_value);
        SI2C_ACK(dev, 0);
    }
    FW_Delay_Ms(dev->D1);
    
    for(i = 0; i < num; i++)
    {
        SI2C_Write_Byte(dev, *pdata++);
//        SI2C_ACK(dev, 0);
//        FW_Delay_Ms(dev->D2);
    }
    
    SI2C_Stop(dev);
    FW_Delay_Ms(dev->D3);
    
    return num;
    #endif
}

__INLINE_STATIC_ u32  FW_SI2C_Read(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num)
{
    #if 0
    u32 i;
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID);
    SI2C_ACK(dev, 0);
    
    SI2C_Write_Byte(dev, addr);
    SI2C_ACK(dev, 0);
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID + 1);
    SI2C_ACK(dev, 0);
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = SI2C_Read_Byte(dev);
    }
    
    SCL_OUT(dev, LEVEL_L);
    SI2C_ACK(dev, 1);
    
    SI2C_Stop(dev);
    
    return num;
    
    #else
    u32 i;
    u8 addr_num;
    u8 addr_value;
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID);
    SI2C_ACK(dev, 0);
    FW_Delay_Ms(dev->D0);
    
    addr_num = (u8)((addr >> 24) + 1);
    for(i = 0; i < addr_num; i++)
    {
        addr_value = (u8)((addr >> (i << 3)) & 0xFF);
        SI2C_Write_Byte(dev, addr_value);
        SI2C_ACK(dev, 0);
    }
    FW_Delay_Ms(dev->D1);
    
    SI2C_Start(dev);
    
    SI2C_Write_Byte(dev, dev->ID | 0x01);
    SI2C_ACK(dev, 0);
    FW_Delay_Ms(dev->D0);
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = SI2C_Read_Byte(dev);
//        SI2C_ACK(dev, 0);
//        FW_Delay_Ms(dev->D2);
    }
    
    SCL_OUT(dev, LEVEL_L);
    SI2C_ACK(dev, 1);
    
    SI2C_Stop(dev);
    FW_Delay_Ms(dev->D3);
    
    return num;
    #endif
}

//void FW_SI2C_SetDelay(FW_I2C_Type *dev, u8 d0, u8 d1, u8 d2, u8 d3)
//{
//    dev->D0 = d0;
//    dev->D1 = d1;
//    dev->D2 = d2;
//    dev->D3 = d3;
//}

#else
void FW_SI2C_Write(FW_I2C_Type *dev, u8 *paddr, u8 num_addr, u8 *pdata, u32 num)
{
    u32 i;
    
    Start(dev);
    
    Write_Byte(dev, dev->ID);
    ACK(dev, 0);
    
    FW_Delay_Ms(dev->D0);
    
    for(i = 0; i < num_addr; i++)
    {
        Write_Byte(dev, paddr[num_addr - 1 - i]);
        ACK(dev, 0);
    }
    FW_Delay_Ms(dev->D1);
    
    for(i = 0; i < num; i++)
    {
        Write_Byte(dev, *pdata++);
        ACK(dev, 0);
        FW_Delay_Ms(dev->D2);
    }
    
    Stop(dev);
    
    FW_Delay_Ms(dev->D3);
}

u32  FW_SI2C_Read(FW_I2C_Type *dev, u8 *paddr, u8 num_addr, u8 *pdata, u32 num)
{
    u32 i;
    
    Start(dev);
    
    Write_Byte(dev, dev->ID);
    ACK(dev, 0);
    FW_Delay_Ms(dev->D0);
    
    for(i = 0; i < num_addr; i++)
    {
        Write_Byte(dev, paddr[num_addr - 1 - i]);
        ACK(dev, 0);
    }
    FW_Delay_Ms(dev->D1);
    
    Start(dev);
    
    Write_Byte(dev, dev->ID + 1);
    ACK(dev, 0);
    FW_Delay_Ms(dev->D0);
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = Read_Byte(dev);
        ACK(dev, 0);
        FW_Delay_Ms(dev->D2);
    }
    
    SCL_OUT(dev, LEVEL_L);
    ACK(dev, 1);
    Stop(dev);
    FW_Delay_Ms(dev->D3);
    
    return num;
}
#endif




/* IO Simular I2C */
__CONST_STATIC_ FW_I2C_Driver_Type SI2C_Driver =
{
    .Init = FW_SI2C_Init,
//    .TX_Byte = FW_SI2C_WriteByte,
//    .RX_Byte = FW_SI2C_ReadByte,
    .Write = FW_SI2C_Write,
    .Read = FW_SI2C_Read,
};
FW_DRIVER_REGIST("io->i2c", &SI2C_Driver, SI2C);




