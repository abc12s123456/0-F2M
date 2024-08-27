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
#include "fw_ow.h"


#if defined(OW_MOD_EN) && OW_MOD_EN


#include "fw_gpio.h"
#include "fw_delay.h"


/**
 * 1-wire延时值，来源：analog官网
 */
#define A  6
#define B  64
#define C  60
#define D  10
#define E  9
#define F  55
#define G  0
#define H  480
#define I  70
#define J  410


__INLINE_STATIC_ void FW_OW_WH(u16 pin)
{
    FW_GPIO_CLR(pin);
    FW_Delay_Us(5);
    FW_GPIO_SET(pin);
    FW_Delay_Us(60);
}

__INLINE_STATIC_ void FW_OW_WL(u16 pin)
{
    FW_GPIO_CLR(pin);
    FW_Delay_Us(60);
    FW_GPIO_SET(pin);
    FW_Delay_Us(5);
}


__INLINE_STATIC_ void FW_OW_WriteByte(FW_OW_Type *dev, u8 value)
{
    u16 pin = dev->Pin;
    u8 i;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    for(i =0; i < 8; i++)
    {
        if(value & 0x01)  FW_OW_WH(pin);
        else  FW_OW_WL(pin);
        value >>= 1;
    }
}

__INLINE_STATIC_ u8   FW_OW_ReadByte(FW_OW_Type *dev)
{
    u16 pin = dev->Pin;
    u8 i, value = 0;
    
    for(i = 0; i < 8; i++)
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        
        FW_GPIO_CLR(pin);
        FW_Delay_Us(5);
        FW_GPIO_SET(pin);
        FW_Delay_Us(5);
        
        FW_GPIO_Init(pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
        
        value >>= 1;         //由最低位开始读
        if(FW_GPIO_Read(pin) == LEVEL_H)  value |= 0x80;
        FW_Delay_Us(60);
    }
    
    return value;
}


Bool FW_OW_Init(FW_OW_Type *dev)
{
    u16 pin = dev->Pin;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);

    FW_GPIO_CLR(pin);        //L
    FW_Delay_Us(750);        //480~960us
    FW_GPIO_SET(pin);        //H，释放总线
    FW_Delay_Us(60);         //15~60us
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    
    /* 在从器件接收到主机发送的复位信号后，等待15~60us，向主机发送一个由60~240us
       低电平信号构成的存在脉冲 */
    if(FW_GPIO_Read(pin) == LEVEL_H)  return False;
    return True;
}

void FW_OW_Reset(FW_OW_Type *dev)
{
    u16 pin = dev->Pin;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    FW_GPIO_CLR(pin);
    FW_Delay_Us(750);
    FW_GPIO_SET(pin);
    FW_Delay_Us(60);
}

Bool FW_OW_WaitACK(FW_OW_Type *dev)
{
    u16 pin = dev->Pin;
    u8 cnt = 0;
    
    FW_GPIO_Init(pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    
    while(FW_GPIO_Read(pin) == LEVEL_H)
    {
        if(cnt++ >= 20)  return False;
        FW_Delay_Us(10);
    }
    
    cnt = 0;
    while(FW_GPIO_Read(pin) == LEVEL_L)
    {
        if(cnt++ >= 24)  return False;
        FW_Delay_Us(10);
    }
    
    return True;
}

u32  FW_OW_Write(FW_OW_Type *dev, const u8 *pdata, u32 num)
{
    u32 i;
    
    for(i = 0; i < num; i++)
    {
        FW_OW_WriteByte(dev, *pdata++);
    }
    
    return num;
}

u32  FW_OW_Read(FW_OW_Type *dev, u8 *pdata, u32 num)
{
    u32 i;
    
    for(i = 0; i < num; i++)
    {
        *pdata++ = FW_OW_ReadByte(dev);
    }
    
    return num;
}


#endif  /* defined(OW_MOD_EN) && OW_MOD_EN */

