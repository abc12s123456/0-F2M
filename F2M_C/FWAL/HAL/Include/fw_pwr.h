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
 
#ifndef _FW_PWR_H_
#define _FW_PWR_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


#define PWR_DRV_NUM               0


/* 低功耗模式 */
typedef enum
{
    FW_LPM_Normal = 0,            //工作模式
    FW_LPM_Sleep,                 //睡眠模式
    FW_LPM_Stop,                  //停止模式(深度睡眠)
    FW_LPM_Standby,               //待机模式
}FW_LPM_Enum;


typedef enum
{
    FW_LVDT_1V0 = 10,
    FW_LVDT_1V1,
    FW_LVDT_1V2,
    FW_LVDT_1V3,
    FW_LVDT_1V4,
    FW_LVDT_1V5,
    FW_LVDT_1V6,
    FW_LVDT_1V7,
    FW_LVDT_1V8,
    FW_LVDT_1V9,
    FW_LVDT_2V0,
    FW_LVDT_2V1,
    FW_LVDT_2V2,
    FW_LVDT_2V3,
    FW_LVDT_2V4,
    FW_LVDT_2V5,
    FW_LVDT_2V6,
    FW_LVDT_2V7,
    FW_LVDT_2V8,
    FW_LVDT_2V9,
    FW_LVDT_3V0,
    FW_LVDT_3V1,
    FW_LVDT_3V2,
    FW_LVDT_3V3,
    FW_LVDT_3V4,
    FW_LVDT_3V5,
    FW_LVDT_3V6,
    FW_LVDT_3V7,
    FW_LVDT_3V8,
    FW_LVDT_3V9,
    FW_LVDT_4V0,
    FW_LVDT_4V1,
    FW_LVDT_4V2,
    FW_LVDT_4V3,
    FW_LVDT_4V4,
    FW_LVDT_4V5,
}FW_LVDT_Enum;


typedef struct
{
    void (*IH_CB)(void *);
    void *IH_Pdata;
    u32  LVDT : 8;
    u32  : 24;
}FW_PWR_Type;


typedef struct
{
    void (*Init)(void);
    void (*SetLPM)(FW_LPM_Enum lpm);
}FW_PWR_Driver_Type;


void FW_PWR_DeInit(void);
void FW_PWR_Init(void);
void FW_PWR_SetLPM(FW_LPM_Enum lpm);

FW_PWR_Type *FW_PWR_GetDevice(void);

void FW_PWR_SetLVDT(FW_LVDT_Enum lvdt);
void FW_PWR_SetCB(void (*cb)(void *), void *pdata);

void FW_PWR_IH(void);


#ifdef __cplusplus
}
#endif

#endif

