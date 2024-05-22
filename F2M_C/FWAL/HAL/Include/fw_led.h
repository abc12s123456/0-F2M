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
 
#ifndef _FW_LED_H_
#define _FW_LED_H_

#ifdef __cplusplus
extern "{"
#endif


#include "fw_device.h"
#include "stiming.h"


typedef struct
{
    FW_Device_Type Parent;
    
    u32 Pin : 16;                      //LED驱动IO
    
    u32 Level : 1;                     //LED打开电平，LEVEL_L/H
    u32 State : 1;                     //LED工作状态，ON: 亮；OFF: 灭
    
    u32 : 14;
    
    STiming_Type *Timing;
}FW_LED_Type;


void FW_LED_Init(FW_LED_Type *pdev);

void FW_LED_SetPin(FW_LED_Type *pdev, u16 pin);
u16  FW_LED_GetPin(FW_LED_Type *pdev);
void FW_LED_SetLevel(FW_LED_Type *pdev, u8 level);
u8   FW_LED_GetLevel(FW_LED_Type *pdev);

void FW_LED_ON(FW_LED_Type *pdev);
void FW_LED_OFF(FW_LED_Type *pdev);

u8   FW_LED_GetState(FW_LED_Type *pdev);

void FW_LED_Shine(FW_LED_Type *pdev, u16 period, u8 times);
void FW_LED_Blink(FW_LED_Type *pdev, u16 period);
void FW_LED_AdjBlink(FW_LED_Type *pdev, u16 period, u8 dc);

void FW_LED_Breath(FW_LED_Type *pdev, u16 period);
void FW_LED_ExqBreath(FW_LED_Type *pdev, u16 period);


#ifdef __cplusplus
}
#endif

#endif

