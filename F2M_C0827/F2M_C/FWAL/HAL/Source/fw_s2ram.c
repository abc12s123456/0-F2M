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
#include "fw_s2ram.h"


#if defined(S2RAM_MOD_EN) && S2RAM_MOD_EN


#include "fw_debug.h"

#include "fw_system.h"


void FW_S2RAM_Init(FW_S2RAM_Type *dev)
{
    FW_S2RAM_Driver_Type *drv;
    
    FW_ASSERT(dev);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Init);
    
    if(drv->Init)  drv->Init(dev);
}

u32  FW_S2RAM_GetBase(FW_S2RAM_Type *dev)
{
    return dev->Base;
}

u32  FW_S2RAM_GetSize(FW_S2RAM_Type *dev)
{
    return dev->Size;
}

void FW_S2RAM_Clear(FW_S2RAM_Type *dev)
{
    FW_S2RAM_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Clear);
    
    drv->Clear(dev);
}

u32  FW_S2RAM_Write(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num)
{
    FW_S2RAM_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(addr >= dev->Size)
    {
        return 0;
    }
    
    if(num > dev->Size - addr)
    {
        return 0;
    }
    
    FW_ASSERT(drv->Write);
    
    return drv->Write(dev, addr + dev->Base, pdata, num);
}

u32  FW_S2RAM_Read(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num)
{
    FW_S2RAM_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(addr >= dev->Size)
    {
        return 0;
    }
    
    if(num > dev->Size - addr)
    {
        return 0;
    }
    
    FW_ASSERT(drv->Read);
    
    return drv->Read(dev, addr + dev->Base, pdata, num);
}


#endif  /* defined(S2RAM_MOD_EN) && S2RAM_MOD_EN */

