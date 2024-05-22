#ifndef _FW_S2RAM_H_
#define _FW_S2RAM_H_

#ifdef __cplusplus
extern "C"{
#endif


/*
ram that supports resetting
*/


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    u32 Base;
    u32 Size;
}FW_S2RAM_Type;


typedef struct
{
    void (*Init)(FW_S2RAM_Type *dev);
    void (*Clear)(FW_S2RAM_Type *dev);
    u32  (*Write)(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num);
    u32  (*Read)(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num);
}FW_S2RAM_Driver_Type;


void FW_S2RAM_Init(FW_S2RAM_Type *dev);

u32  FW_S2RAM_GetBase(FW_S2RAM_Type *dev);
u32  FW_S2RAM_GetSize(FW_S2RAM_Type *dev);

void FW_S2RAM_Clear(FW_S2RAM_Type *dev);

u32  FW_S2RAM_Write(FW_S2RAM_Type *dev, u32 addr, const void *pdata, u32 num);
u32  FW_S2RAM_Read(FW_S2RAM_Type *dev, u32 addr, void *pdata, u32 num);


#ifdef __cplusplus
}
#endif

#endif

