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
#ifndef _TM7711_H_
#define _TM7711_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


//typedef enum
//{
//    TM77_CH_10Hz = 1,
//    TM77_CH_40Hz,
//    TM77_CH_Temp,
//}TM77_CH_Enum;


typedef struct
{
    FW_Device_Type Device;
    u16 PD_SCK_Pin : 16;                          //掉电模式和串口时钟输入
    u16 DOUT_Pin : 16;                            //串口数据输出
    u16 Channel : 2;                              //转换通道
    u16 : 14;
}TM77_Type;


//void TM77_Init(TM77_Type *dev);
//s32  TM77_Get10Hz(TM77_Type *dev);
//s32  TM77_GetTemp(TM77_Type *dev);
//s32  TM77_Get40Hz(TM77_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

