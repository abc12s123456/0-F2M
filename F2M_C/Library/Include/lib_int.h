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
#ifndef _LIB_INT_H_
#define _LIB_INT_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


s32 Int_Max(s32 num1, s32 num2);
s32 Int_Min(s32 num1, s32 num2);

u8  Itoa(s32 num, char *pdata);
u8  Htoa(u32 value, char *pdata);

u8  Int_GetBits(u32 num, u8 value);

u32 Int32_EndianExchange(u32 value);
u16 Int16_EndianExchange(u16 value);

u32 Int_ReadBuffer(void *pdata, u32 index, u8 width);
u32 Int32_ReadBuffer(void *pdata, u32 index);
u32 Int16_ReadBuffer(void *pdata, u32 index);
u32 Int8_ReadBuffer(void *pdata, u32 index);

u8  Int8_GetXor(void *pdata, u8 start_value, u32 num);
u8  Int8_GetSum(void *pdata, u8 start_value, u32 num);

typedef struct
{
    u32 Sum;                                //和值
    u32 Value;                              //有效值
    u32 Times : 16;                         //采样次数(初始化时设置)
    u32 Count : 16;                         //计数
}Int_Average_Type;

void Int_Average_Init(Int_Average_Type *ave, u16 times);
u32  Int_Average_Get(Int_Average_Type *ave, u32 value);


#ifdef __cplusplus
}
#endif

#endif

