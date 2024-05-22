#ifndef _WF183D_H_
#define _WF183D_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct WF183D WF183D_Type;

struct WF183D
{
    FW_Device_Type Device;
    
    u32 OWI_Pin : 16;
    u32 : 16;
    
    void (*Init)(WF183D_Type *dev);
    u8   (*Get_Temprature)(WF183D_Type *dev, s32 *pdata);
    u8   (*Get_Pressure)(WF183D_Type *dev, s32 *pdata);
};


#ifdef __cplusplus
}
#endif

#endif

