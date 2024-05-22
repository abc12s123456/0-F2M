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
 
#ifndef _FW_USB_H_
#define _FW_USB_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    void (*IH_CB)(void *);
    void *IH_Pdata;
    
    u32 EN_Pin : 16;
    u32 EN_VL : 1;
    
    u32 Wakeup_EN : 1;
    u32 Connect_Flag : 1;
    u32 Receive_Flag : 1;
    
    u32 : 12;
}FW_USB_Type;


typedef struct FW_USB_Driver
{
    void (*Init)(FW_USB_Type *dev);
    
    u32  (*Write)(FW_USB_Type *dev, u32 offset, const u8 *pdata, u32 num);
    u32  (*Read)(FW_USB_Type *dev, u32 offset, u8 *pdata, u32 num);
    
    void (*Connect)(FW_USB_Type *dev);
    void (*Disconnect)(FW_USB_Type *dev);
}FW_USB_Driver_Type;


void FW_USB_Init(FW_USB_Type *dev);



#ifdef __cplusplus
}
#endif

#endif

