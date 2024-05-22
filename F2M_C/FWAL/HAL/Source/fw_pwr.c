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
 
#include "fw_pwr.h"
#include "fw_debug.h"


static FW_PWR_Driver_Type *Driver;
static FW_PWR_Type PWR;


extern void FW_Lock(void);
extern void FW_Unlock(void);


void FW_PWR_DeInit(void)
{
    FW_PWR_Type *dev = &PWR;
    
    dev->IH_CB = NULL;
    dev->IH_Pdata = NULL;
    dev->LVDT = 0;
}

void FW_PWR_Init(void)
{
    Driver = FW_Driver_Find("ll->pwr");
    if(Driver == NULL)
    {
        /* pwr driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->Init);
    Driver->Init();
}

void FW_PWR_SetLPM(FW_LPM_Enum lpm)
{
    FW_ASSERT(Driver->SetLPM);
    Driver->SetLPM(lpm);
}

FW_PWR_Type *FW_PWR_GetDevice(void)
{
    return &PWR;
}

void FW_PWR_SetLVDT(FW_LVDT_Enum lvdt)
{
    FW_PWR_Type *dev = &PWR;
    dev->LVDT = lvdt;
}

void FW_PWR_SetCB(void (*cb)(void *), void *pdata)
{
    FW_PWR_Type *dev = &PWR;
    FW_Lock();
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
    FW_Unlock();
}

void FW_PWR_IH(void)
{
    FW_PWR_Type *dev = &PWR;
    if(dev->IH_CB)  dev->IH_CB(dev->IH_Pdata);
}

