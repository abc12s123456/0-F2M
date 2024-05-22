#ifndef _DS18B20_H_
#define _DS18B20_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* 采样精度 */
typedef enum
{
    DS18B20_ACC_0_5 = 0x1F,       //0.5℃
    DS18B20_ACC_0_25 = 0x3F,      //
    DS18B20_ACC_0_125 = 0x5F,
    DS18B20_ACC_0_0625 = 0x7F,
    DS18B20_ACC_Default = 0x7F,
}DS18B20_ACC_Enum;


typedef struct DS18B20  DS18B20_Type;

struct DS18B20
{
    FW_Device_Type Device;
    
    u32 Accuracy : 8;
    
    u32 Alarm_H : 8;
    u32 Alarm_L : 8;
    
    u32 : 8;
};


#ifdef __cplusplus
}
#endif

#endif

