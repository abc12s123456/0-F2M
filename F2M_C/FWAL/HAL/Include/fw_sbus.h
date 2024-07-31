#ifndef _FW_SBUS_H_
#define _FW_SBUS_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    u32 Pin : 16;
}FW_SBus_Type;


Bool FW_SBus_Init(FW_SBus_Type *dev);
u32  FW_SBus_Write(FW_SBus_Type *dev, const u8 *pdata, u32 num);
u32  FW_SBus_Read(FW_SBus_Type *dev, u8 *pdata, u32 num);


#ifdef __cplusplus
}
#endif

#endif

