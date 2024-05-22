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

