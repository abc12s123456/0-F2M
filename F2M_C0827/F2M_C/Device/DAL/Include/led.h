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
#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "stiming.h"


typedef struct LED
{
    FW_Device_Type Device;
    
    void (*Init)(struct LED *dev);
    void (*CTL)(struct LED *dev, u8 state);
    
    u32  Pin : 16;
    u32  Level : 1;
    u32  State : 1;
    
    u32  : 14;
    
    STiming_Type *Timing;
}LED_Type;


void LED_DeInit(LED_Type *dev);
void LED_Init(LED_Type *dev);

void LED_ON(LED_Type *dev);
void LED_OFF(LED_Type *dev);

void LED_Shine(LED_Type *dev, u16 period, u8 times);
void LED_Blink(LED_Type *dev, u16 period);
void LED_Adj_Blink(LED_Type *dev, u16 period, u8 dc);

void LED_Breath(LED_Type *dev, u16 period);
void LED_Exq_Breath(LED_Type *dev, u16 period);


#ifdef __cplusplus
}
#endif

#endif

