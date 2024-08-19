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
#ifndef _FW_BKP_H_
#define _FW_BKP_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_mod_conf.h"


#if defined(BKP_MOD_EN) && BKP_MOD_EN


#include "fw_device.h"

    
typedef enum
{
    FW_BKP_State_OK = 0,
    FW_BKP_State_AddrErr,
    FW_BKP_State_DataErr,
    FW_BKP_State_ArgsErr,
}FW_BKP_State_Enum;


typedef struct
{
    void (*Init)(void);
    
    u32  (*Get_Base)(void);
    u32  (*Get_Size)(void);
    
    u32  (*Write)(u32 addr, const void *pdata, u32 num);
    u32  (*Read)(u32 addr, void *pdata, u32 num);
}FW_BKP_Driver_Type;


/**
 * @API
 */
void FW_BKP_Init(void);

u32  FW_BKP_GetBase(void);
u32  FW_BKP_GetSize(void);

u32  FW_BKP_Write(u32 addr, const void *pdata, u32 num);
u32  FW_BKP_Read(u32 addr, void *pdata, u32 num);


#endif  /* defined(BKP_MOD_EN) && BKP_MOD_EN */

#ifdef __cplusplus
}
#endif

#endif

