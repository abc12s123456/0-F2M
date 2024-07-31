#ifndef _FW_OW_H_
#define _FW_OW_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    u32 Pin : 16;
    
    u32 : 16;
}FW_OW_Type;


Bool FW_OW_Init(FW_OW_Type *dev);

void FW_OW_Reset(FW_OW_Type *dev);
Bool FW_OW_WaitACK(FW_OW_Type *dev);

u32  FW_OW_Write(FW_OW_Type *dev, const u8 *pdata, u32 num);
u32  FW_OW_Read(FW_OW_Type *dev, u8 *pdata, u32 num);


#ifdef __cplusplus
}
#endif

#endif

