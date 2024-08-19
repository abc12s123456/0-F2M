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
#include "fw_sbus.h"


#if defined(SBUS_MOD_EN) && SBUS_MOD_EN


#include "fw_gpio.h"
#include "fw_delay.h"


__INLINE_STATIC_ void FW_SBus_WriteByte(FW_SBus_Type *dev, u8 value)
{
    u16 pin = dev->Pin;
    u8 i;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(pin);        //H，为写入做准备
    FW_Delay_Us(5);
    
    for(i =0; i < 8; i++)
    {
        FW_GPIO_CLR(pin);    //L，产生写时序
        FW_Delay_Us(5);
        if(value & 0x01)  FW_GPIO_SET(pin);
        FW_Delay_Us(30);     //15~60us内等待应答
        FW_GPIO_SET(pin);    //H，释放总线
        value >>= 1;
    }
}

__INLINE_STATIC_ u8   FW_SBus_ReadByte(FW_SBus_Type *dev)
{
    u16 pin = dev->Pin;
    u8 i, value = 0;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(pin);        //H
    FW_Delay_Us(5);
    
    for(i = 0; i < 8; i++)
    {
        FW_GPIO_CLR(pin);
        FW_Delay_Us(5);
        
        value >>= 1;         //由最低位开始读
        FW_GPIO_Init(pin, FW_GPIO_Mode_IPN, FW_GPIO_Speed_Low);
        if(FW_GPIO_Read(pin) == LEVEL_H)  value |= 0x80;
        FW_Delay_Us(30);
        
        FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
        FW_GPIO_SET(pin);
        FW_Delay_Us(5);
    }
    
    return value;
}


Bool FW_SBus_Init(FW_SBus_Type *dev)
{
    u16 pin = dev->Pin;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    
    FW_GPIO_SET(pin);
    FW_Delay_Us(10);
    
    FW_GPIO_CLR(pin);        //L
    FW_Delay_Us(600);        //480~960us
    FW_GPIO_SET(pin);        //H，释放总线
    FW_Delay_Us(30);         //15~60us
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_IPN, FW_GPIO_Speed_Low);
    
    if(FW_GPIO_Read(pin) == LEVEL_H)  return False;
    return True;
}

u32  FW_SBus_Write(FW_SBus_Type *dev, const u8 *pdata, u32 num)
{
    u32 i;
    
    for(i = 0; i < num; i++)
    {
        FW_SBus_WriteByte(dev, *pdata++);
    }
    
    return num;
}

u32  FW_SBus_Read(FW_SBus_Type *dev, u8 *pdata, u32 num)
{
    u32 i;
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = FW_SBus_ReadByte(dev);
    }
    
    return num;
}


#endif  /* defined(SBUS_MOD_EN) && SBUS_MOD_EN */

