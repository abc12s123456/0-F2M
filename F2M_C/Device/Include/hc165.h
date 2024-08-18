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
#ifndef _HC165_H_
#define _HC165_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct HC165 HC165_Type;


struct HC165
{
    FW_Device_Type Device;
    
    u32 PL_Pin : 16;                   //异步并行负载输入控制，低有效
    u32 CLK_Pin : 16;                  //时钟
    u32 EN_Pin : 16;                   //使能，高有效
    u32 Data_Pin : 16;                 //串行数据
    
    void (*Init)(HC165_Type *dev);
    void (*CTL)(HC165_Type *dev, u8 state);
    u32  (*Read)(HC165_Type *dev, u8 *pdata, u32 num);
};


#ifdef __cplusplus
}
#endif

#endif

