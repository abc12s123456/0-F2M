/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
 
#ifndef _SENSOR_H_
#define _SENSOR_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/*
由于传感器类型众多，使用FW_DRIVER_REGIST方式，无法区分相同系列但驱动不一致的设备，
例如一个项目中使用多个类型的传感器(绝压、差压等)，则无法使用FW_Driver_Find(drv_id)
获取正确的驱动。所以对于传感器设备，每一个型号实现一个Sensor_XXX_Init(sensor_name)
用于绑定具体的驱动实例。
*/



/* 传感器类型 */
typedef enum
{
    Sensor_Null = 0,
//    Sensor_Value,            //返回采集值，无需转换为具体量纲
    Sensor_Temprature,       //温度，uint: ℃
    Sensor_Humidity,         //湿度，uint: %RH
    Sensor_Pressure,         //压力，uint: Pa
    Sensor_Light,            //光强，uint: lux
    Sensor_Force,            //力(拉力、重力……)，uint: mN
}Sensor_Enum;


/* 传感器返回值状态 */
typedef enum
{
    Sensor_RS_OK = 0,
    Sensor_RS_Fail,
    Sensor_RS_VerifyErr,
    Sensor_RS_NoData,
    Sensor_RS_NoDev,
}Sensor_RS_Enum;


/* 传感器工作模式 */
typedef enum
{
    Sensor_Mode_Continue = 0,  //持续发送
    Sensor_Mode_Trigger,       //触发方式
}Sensor_Mode_Enum;


typedef struct
{
    FW_Device_Type Device;
    
    u32 Type : 8;            //传感器类型，参照Sensor_Enum
    u32 ODR : 16;            //out data rate，单位：0.1HZ
    u32 Mode : 2;
    
    u32 : 6;
    
    s32 Min;                 //检测值范围[min : max]
    s32 Max;
    
    void *Buffer;            //
}Sensor_Type;


typedef struct
{
    void (*Init)(Sensor_Type *dev);
    void (*Reset)(Sensor_Type *dev);
    
//    u8   (*Get_Type)(Sensor_Type *dev);
    
    void (*Set_Period)(Sensor_Type *dev, u16 period);        //out data period
//    u32  (*Get_ODR)(Sensor_Type *dev);
    
//    void (*Set_Range)(Sensor_Type *dev, s32 min, s32 max);
    
//    void (*CTL)(Sensor_Type *dev, u8 state);
    void (*Sleep)(Sensor_Type *dev, u32 timeout);
    
    u8   (*Get_Temprature)(Sensor_Type *dev, s32 *pdata);
    u8   (*Get_Sample)(Sensor_Type *dev, s32 *pdata);
    s32  (*Formula)(s32 value);
}Sensor_Driver_Type;


void Sensor_Init(Sensor_Type *dev);

void Sensor_Reset(Sensor_Type *dev);
void Sensor_Set_Period(Sensor_Type *dev, u16 period);

u8   Sensor_Get_Temprature(Sensor_Type *dev, s32 *pdata);
u8   Sensor_Get_Sample(Sensor_Type *dev, s32 *pdata);
s32  Sensor_Formula(Sensor_Type *dev, s32 value);


#ifdef __cplusplus
}
#endif

#endif

