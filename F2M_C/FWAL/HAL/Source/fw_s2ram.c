#include "fw_s2ram.h"
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

