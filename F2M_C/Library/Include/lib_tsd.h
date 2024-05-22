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
 
#ifndef _LIB_TSD_H_
#define _LIB_TSD_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


typedef struct
{
    float Max;
    float Min;
    
    float Range;
    
    u32   Times : 16;
    u32   Increase_Count : 8;
    u32   Reduce_Count : 8;
    
    float Sum;
    float Value;
}TSD_Type;


void TSD_Init(TSD_Type *tsd, float max, float min, float range, u16 times);
float TSD_GetValue(TSD_Type *tsd, float value);
float TSD_GetAverage(TSD_Type *tsd, float value);


#ifdef __cplusplus
}
#endif

#endif

