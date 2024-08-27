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
#ifndef _MBLOCK_H_
#define _MBLOCK_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


typedef struct
{
    u32 Addr;
    void *Pdata;
    u32 Num;
    
    /* 粒度大小的缓存，至少是粒度数量字节对齐 */
    u8  *Buffer;
    
    /* 粒度大小，以底层允许读写的最小单元中的大者为准 */
    u32 Gran_Size;
    
    /* 粒度读写，由底层实现 */
    u32 (*Write_Gran)(u32 addr, const void *pdata);
    u32 (*Read_Gran)(u32 addr, void *pdata);
}MBlock_Type;


u32 MBlock_Write_SelfAlign(MBlock_Type *mb);
u32 MBlock_Read_SelfAlign(MBlock_Type *mb);


#ifdef __cplusplus
}
#endif

#endif

