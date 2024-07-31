#ifndef _TM7711_H_
#define _TM7711_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


//typedef enum
//{
//    TM77_CH_10Hz = 1,
//    TM77_CH_40Hz,
//    TM77_CH_Temp,
//}TM77_CH_Enum;


typedef struct
{
    FW_Device_Type Device;
    u16 PD_SCK_Pin : 16;                          //掉电模式和串口时钟输入
    u16 DOUT_Pin : 16;                            //串口数据输出
    u16 Channel : 2;                              //转换通道
    u16 : 14;
}TM77_Type;


//void TM77_Init(TM77_Type *dev);
//s32  TM77_Get10Hz(TM77_Type *dev);
//s32  TM77_GetTemp(TM77_Type *dev);
//s32  TM77_Get40Hz(TM77_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

