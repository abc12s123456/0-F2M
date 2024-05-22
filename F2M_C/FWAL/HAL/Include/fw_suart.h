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
 
#ifndef _FW_SUART_H_
#define _FW_SUART_H_

#ifdef __cplusplus
extern "C"{
#endif


/* 模拟要实现的是uart驱动的一个实例 */
#include "fw_device.h"


typedef struct
{
    __RO_ FW_Device_Type Parent;
    
    /* IO */
    u16 TX_Pin;
    u16 RX_Pin;
    
    /* 波特率 */
    u32 Baudrate;
    
    /* 数据帧格式 */
    u32 Data_Bits : 4;
    u32 Stop_Bits : 4;
    u32 Parity :4;
    
    /* 模拟UART的延时时间，单位：us */
    u32 SDTime : 16;
    u32 : 4;
}FW_SUART_Type;


void FW_SUART_Init(FW_SUART_Type *pdev);
u32  FW_SUART_Write(FW_SUART_Type *pdev, const u8 *pdata, u32 num);
u32  FW_SUART_Read(FW_SUART_Type *pdev, u8 *pdata, u32 num);

void FW_SUART_PrintInit(FW_SUART_Type *pdev);
int  FW_SUART_Printf(const char *format, ...);


#ifdef __cplusplus
}
#endif

#endif

