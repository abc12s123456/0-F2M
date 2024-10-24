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
 
#ifndef _GYY0709_H_
#define _GYY0709_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    WM_OM_Stop = 0x00,       //报警停止
    WM_OM_LL   = 0x02,       //低音量，低优先级
    WM_OM_LM   = 0x04,       //低音量，中优先级
    WM_OM_LH   = 0x06,       //低音量，高优先级
    WM_OM_HL   = 0x03,       //高音量，低优先级
    WM_OM_HM   = 0x05,       //高音量，中优先级
    WM_OM_HH   = 0x07,       //高音量，高优先级
}WM_OM_Enum;


typedef struct WM WM_Type;

struct WM
{
    FW_Device_Type Device;
    
    u32 L1_Pin : 16;  
    u32 L2_Pin : 16;
    u32 L3_Pin : 16;
    
    u32 : 16;
    
    void (*Init)(WM_Type *dev);
    void (*Output)(WM_Type *dev, WM_OM_Enum om);
};


void WM_Init(WM_Type *dev);
void WM_Output(WM_Type *dev, WM_OM_Enum om);


#ifdef __cplusplus
}
#endif

#endif

