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
#include "fw_bkp.h"
#include "fw_debug.h"
#include "fw_print.h"


static FW_BKP_Driver_Type *Driver;


void FW_BKP_Init(void)
{
    Driver = FW_Driver_Find("ll->bkp");
    if(Driver == NULL)
    {
        /* backup reg driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->Init);
    FW_ASSERT(Driver->Get_Size);
    FW_ASSERT(Driver->Get_Base);
    FW_ASSERT(Driver->Write);
    FW_ASSERT(Driver->Read);
    
    Driver->Init();
}

u32  FW_BKP_GetBase(void)
{
    if(Driver->Get_Base == NULL)
    {
        return 0;
    }
    
    return Driver->Get_Base();
}

u32  FW_BKP_GetSize(void)
{
    if(Driver->Get_Size == NULL)
    {
        return 0;
    }
    
    return Driver->Get_Size();
}

u32  FW_BKP_Write(u32 addr, const void *pdata, u32 num)
{
    if(addr >= Driver->Get_Size())
    {
        return FW_BKP_State_AddrErr;
    }
    
    if(num > Driver->Get_Size() - addr)
    {
        return FW_BKP_State_ArgsErr;
    }
    
    addr += Driver->Get_Base();
    
    return Driver->Write(addr, pdata, num);
}

u32  FW_BKP_Read(u32 addr, void *pdata, u32 num)
{
    if(addr >= Driver->Get_Size())
    {
        return FW_BKP_State_AddrErr;
    }
    
    if(num > Driver->Get_Size() - addr)
    {
        return FW_BKP_State_ArgsErr;
    }
    
    addr += Driver->Get_Base();
    
    return Driver->Read(addr, pdata, num);
}

