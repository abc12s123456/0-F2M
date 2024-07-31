#ifndef _ESP8266_H_
#define _ESP8266_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef struct ESP8266 ESP8266_Type;

struct ESP8266
{
    FW_Device_Type Device;
    
    u32 RST_Pin : 16;
    u32 EN_Pin : 16;
    
    u32 RST_VL : 1;
    u32 EN_VL :1;
    
    u32 : 30;
    
    
};




#ifdef __cplusplus
}
#endif

#endif

