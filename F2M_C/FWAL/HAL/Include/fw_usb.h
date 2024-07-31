#ifndef _FW_USB_H_
#define _FW_USB_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct
{
    FW_Device_Type Device;
    
    void (*IH_CB)(void *);
    void *IH_Pdata;
    
    u32 EN_Pin : 16;
    u32 EN_VL : 1;
    
    u32 Wakeup_EN : 1;
    u32 Connect_Flag : 1;
    u32 Receive_Flag : 1;
    
    u32 : 12;
}FW_USB_Type;


typedef struct FW_USB_Driver
{
    void (*Init)(FW_USB_Type *dev);
    
    u32  (*Write)(FW_USB_Type *dev, u32 offset, const u8 *pdata, u32 num);
    u32  (*Read)(FW_USB_Type *dev, u32 offset, u8 *pdata, u32 num);
    
    void (*Connect)(FW_USB_Type *dev);
    void (*Disconnect)(FW_USB_Type *dev);
}FW_USB_Driver_Type;


void FW_USB_Init(FW_USB_Type *dev);



#ifdef __cplusplus
}
#endif

#endif

