#ifndef _WF200D_H_
#define _WF200D_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct WF200D WF200D_Type;


struct WF200D
{
    FW_Device_Type Device;
    
    u32 RDY_Pin : 16;
    u32 : 16;
    
    void (*Init)(WF200D_Type *dev);
    void (*Write_Byte)(WF200D_Type *dev, u8 reg_addr, u8 value);
    u8   (*Read_Byte)(WF200D_Type *dev, u8 reg_addr);
};


#ifdef __cplusplus
}
#endif

#endif

