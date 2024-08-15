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
#ifndef _HC595_H_
#define _HC595_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct HC595 HC595_Type;


struct HC595
{
    FW_Device_Type Device;
    
    u16 SER_Pin;             //串行数据输入端
    u16 OE_Pin;              //输出使能端，低电平有效; 在不使用IO控制时，接低电平
    u16 RCK_Pin;             //锁存器输出控制
    u16 SCK_Pin;             //数据输入时钟
    u16 SCLR_Pin;            //移位寄存器清0端，低电平有效；在不使用IO控制时，接高电平
    
    u16 OE_VL : 1;
    u16 SCLK_VL : 1;
    
    u16 : 14;
    
    void (*Init)(HC595_Type *dev);
    void (*CTL)(HC595_Type *dev, u8 state);
    void (*Write)(HC595_Type *dev, const u8 *pdata, u32 num);
};


#ifdef __cplusplus
}
#endif

#endif

