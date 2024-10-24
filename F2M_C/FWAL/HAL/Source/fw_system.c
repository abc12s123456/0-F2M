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
#include "fw_system.h"


#if defined(SYSTEM_MOD_EN) && SYSTEM_MOD_EN


#include "fw_debug.h"
#include "fw_print.h"
#include "fw_driver.h"


static FW_System_Driver_Type *Driver;
static volatile u16 Nest_Count = 0;    /* 中断嵌套计数 */
static volatile u8 IRQ_State;          /* 中断状态，OFF：中断关闭；ON：中断打开 */

#if INEST_MONITOR_EN
static volatile u8 Lock_Count = 0;
static volatile u8 Unlock_Count = 0;
#endif

/* NULL */
__INLINE_STATIC_ void NLock(void){}
__INLINE_STATIC_ void NUnlock(void){}


/* INSTANCE */
__INLINE_STATIC_ void ILock(void)
{
    FW_ASSERT(Driver->Disable_IRQ);
    FW_ASSERT(Driver->Get_IRQState);
    
    /* 在中断嵌套的第一层执行该函数时，需要通过获取中断状态来赋值IRQ_State */
    if(Nest_Count == 0)
    {
        IRQ_State = Driver->Get_IRQState();
    }
    
    /* 代码执行经过该处时，无论是被FW_System_DisableIRQ还是FW_System_EnableIRQ打
       断，都不会影响中断嵌套的执行逻辑 */
    Driver->Disable_IRQ();
    Nest_Count++;
    
#if INEST_MONITOR_EN
    Lock_Count++;
#endif
}

__INLINE_STATIC_ void IUnlock(void)
{
    FW_ASSERT(Driver->Disable_IRQ);
    FW_ASSERT(Driver->Enable_IRQ);
    
#if INEST_MONITOR_EN
    if(Nest_Count == 0)
    {
        Nest_Count = Nest_Count;
    }
#endif
    
    /* 退出中断嵌套的最后一层时，恢复第一层嵌套前的中断状态 */
    if(--Nest_Count == 0)
    {
        if(IRQ_State == ON)
        {
            Driver->Enable_IRQ();
        }
        else
        {
            Driver->Disable_IRQ();
        }
    }

#if INEST_MONITOR_EN
    Unlock_Count++;
#endif
}


static void (*PLock)(void) = NLock;
static void (*PUnlock)(void) = NUnlock;


void FW_System_Init(void)
{
    Driver = FW_Driver_Find("ll->system");
    if(Driver == NULL)
    {
        /* system driver is not instanced */
        while(1);
    }
    
    FW_ASSERT(Driver->Init);
    
    Driver->Init();
    
    PLock = ILock;
    PUnlock = IUnlock;
}

void FW_System_DisableIRQ(void)
{
    FW_ASSERT(Driver->Disable_IRQ);
    
    Driver->Disable_IRQ();
}

void FW_System_EnableIRQ(void)
{
    FW_ASSERT(Driver->Enable_IRQ);
    
    Driver->Enable_IRQ();
}

void FW_System_EnterCritical(void)
{
    PLock();
}

void FW_System_ExitCritical(void)
{
    PUnlock();
}

void FW_SDI(void)
{
    FW_ASSERT(Driver->Disable_IRQ);
    
    Driver->Disable_IRQ();
}

void FW_SEI(void)
{
    FW_ASSERT(Driver->Enable_IRQ);

    Driver->Enable_IRQ();
}

void FW_Enter_Critical(void)
{
    PLock();
}

void FW_Exit_Critical(void)
{
    PUnlock();
}

void FW_Lock(void)
{
    PLock();
}

void FW_Unlock(void)
{
    PUnlock();
}

void FW_System_SetSysclk(FW_Sysclk_Enum mode)
{
    FW_ASSERT(Driver->Set_Sysclk);
    
    Driver->Set_Sysclk(mode);
}

u32  FW_System_GetSysclk(void)
{
    FW_ASSERT(Driver->GetSysclk);
    
    return Driver->Get_Sysclk();
}

void FW_System_SetVectorTable(FW_Vector_Table_Enum vector, u32 offset)
{
    FW_ASSERT(Driver->Set_VectorTable);
    
    Driver->Set_VectorTable(vector, offset);
}

void FW_System_SoftReset(void)
{
    FW_ASSERT(Driver->Soft_Reset);
    
    Driver->Soft_Reset();
}

u8   FW_System_GetRSTSource(void)
{
    FW_ASSERT(Driver->Get_RSTSource);
    
    return Driver->Get_RSTSource();
    
}

void FW_System_ClearRSTSource(void)
{
    FW_ASSERT(Driver->Clear_RSTSource);
    
    Driver->Clear_RSTSource();
}


#endif  /* defined(SYSTEM_MOD_EN) && SYSTEM_MOD_EN */

