#ifndef _IP5310_H_
#define _IP5310_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    IP5310_Residual_L0 = 0x00,    //c <= 3%
    IP5310_Residual_L1 = 0x01,    //3% < c <= 25%
    IP5310_Residual_L2 = 0x03,    //25% < c <= 50%
    IP5310_Residual_L3 = 0x07,    //50% < c <= 75%
    IP5310_Residual_L4 = 0x0F,    //c > 75%
    IP5310_Residual_Unknown = 0xEE,
}IP5310_Residual_Enum;


typedef enum
{
    IP5310_State_Stop = 0,        //未充电
    IP5310_State_Charge,          //正在充电
    IP5310_State_Done,            //已充满
    IP5310_State_Unknown,
}IP5310_State_Enum;


typedef enum
{
    IP5310_Load_Heavy = 0,
    IP5310_Load_Light = 1,
    IP5310_Load_Unknown = 2,
}IP5310_Load_Enum;


typedef struct
{
    FW_Device_Type Device;
    
    u32 WKP_Pin : 16;
    u32 WKP_VL : 1;
    
    u32 : 15;
    
    
}IP5310_Type;


void IP5310_Init(IP5310_Type *dev);

u8   IP5310_Get_Residual(IP5310_Type *dev);
u8   IP5310_Get_State(IP5310_Type *dev);
u8   IP5310_Get_Load(IP5310_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

