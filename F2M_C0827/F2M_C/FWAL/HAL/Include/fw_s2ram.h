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
#ifndef _FW_S2RAM_H_
#define _FW_S2RAM_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_mod_conf.h"


#if defined(S2RAM_MOD_EN) && S2RAM_MOD_EN


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    u32 Base;
    u32 Size;
}FW_S2RAM_Type;


typedef struct
{
    void (*Init)(FW_S2RAM_Type *dev);
    void (*Clear)(FW_S2RAM_Type *dev);
    u32  (*Write)(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num);
    u32  (*Read)(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num);
}FW_S2RAM_Driver_Type;


void FW_S2RAM_Init(FW_S2RAM_Type *dev);

u32  FW_S2RAM_GetBase(FW_S2RAM_Type *dev);
u32  FW_S2RAM_GetSize(FW_S2RAM_Type *dev);

void FW_S2RAM_Clear(FW_S2RAM_Type *dev);

u32  FW_S2RAM_Write(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num);
u32  FW_S2RAM_Read(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num);


#endif  /* defined(S2RAM_MOD_EN) && S2RAM_MOD_EN */

#ifdef __cplusplus
}
#endif

#endif

