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
 
#ifndef _DS18B20_H_
#define _DS18B20_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* 采样精度 */
typedef enum
{
    DS18B20_ACC_0_5 = 0x1F,       //0.5℃
    DS18B20_ACC_0_25 = 0x3F,      //
    DS18B20_ACC_0_125 = 0x5F,
    DS18B20_ACC_0_0625 = 0x7F,
    DS18B20_ACC_Default = 0x7F,
}DS18B20_ACC_Enum;


typedef struct DS18B20  DS18B20_Type;

struct DS18B20
{
    FW_Device_Type Device;
    
    u32 Accuracy : 8;
    
    u32 Alarm_H : 8;
    u32 Alarm_L : 8;
    
    u32 : 8;
};


#ifdef __cplusplus
}
#endif

#endif

