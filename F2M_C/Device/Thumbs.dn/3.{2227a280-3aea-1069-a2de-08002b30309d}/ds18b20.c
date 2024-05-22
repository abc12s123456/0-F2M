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
 
#include "ds18b20.h"

#include "fw_gpio.h"
#include "fw_delay.h"
#include "fw_ow.h"

#include "sensor.h"


#define DS_CMD_CONV          0x44
#define DS_CMD_READ_REG      0xBE
#define DS_CMD_WRITE_REG     0x4E      //写暂存寄存器
#define DS_CMD_COPY_REG      0x48      //拷贝暂存寄存器
#define DS_CMD_RECALL_E2PROM 0xB8      //E2PROM召回
#define DS_CMD_READ_PWR      0xB4      //读取供电模式

#define DS_ROM_READ          0x33      //读取ROM，该命令在总线上只有一个设备时才能使用
#define DS_ROM_MATCH         0x55      //匹配ROM，该命令后，M发送64位编码，只有完全匹配的设备才会响应
#define DS_ROM_SKIP          0xCC      //跳过ROM，主设备向总线上所有的设备发送温度转换指令，则所有设备同时执行温度转换
#define DS_ROM_SEARCH        0xF0
#define DS_ROM_ALARM_SEARCH  0xEC      //告警搜索

#define DS_ALARM_H           0x50
#define DS_ALARM_L           0x00



__INLINE_STATIC_ void DS_Write(FW_OW_Type *ow, const u8 value)
{
    FW_OW_Write(ow, &value, 1);
}

__INLINE_STATIC_ u8   DS_Read(FW_OW_Type *ow)
{
    u8 value;
    FW_OW_Read(ow, &value, 1);
    return value;
}


/*
 * Sensor驱动
 */
__INLINE_STATIC_ void DS_Init(Sensor_Type *sensor)
{
    DS18B20_Type *dev = FW_Device_GetParent(sensor);
    FW_OW_Type *ow = FW_Device_GetParent(dev);
    
    sensor->Type = Sensor_Temprature;
    
    if(dev->Accuracy == 0)
    {
        dev->Accuracy = DS18B20_ACC_Default;
    }
    
    if(dev->Alarm_H <= dev->Alarm_L)
    {
        dev->Alarm_H = DS_ALARM_H;
        dev->Alarm_L = DS_ALARM_L;
    }
    
    if(FW_OW_Init(ow) != True)  return;
//    DS_Write(ow, DS_ROM_SKIP);         //跳过SN操作
//    DS_Write(ow, DS_CMD_WRITE_REG);    //写暂存寄存器
//    DS_Write(ow, dev->Alarm_H);        //写高温报警值
//    DS_Write(ow, dev->Alarm_L);        //写低温报警值
//    DS_Write(ow, dev->Accuracy);       //写测量精度
}

__INLINE_STATIC_ void DS_Reset(Sensor_Type *sensor)
{
    DS18B20_Type *dev = FW_Device_GetParent(sensor);
    FW_OW_Type *ow = FW_Device_GetParent(dev);
    FW_OW_Reset(ow);
}

__INLINE_STATIC_ void DS_Set_Period(Sensor_Type *sensor, u16 period)
{

}

__INLINE_STATIC_ u8   DS_Get_Temprature(Sensor_Type *sensor, s32 *pdata)
{
    DS18B20_Type *dev = FW_Device_GetParent(sensor);
    FW_OW_Type *ow = FW_Device_GetParent(dev);
    u8 i;
    u8 msg[9];
    
    u8 temp_h, temp_l;
    u16 temp;
    u32 temp_int, temp_f;
    u8 acc = dev->Accuracy;
    
//    if(FW_OW_Init(ow) != True)  return Sensor_RS_NoDev;
    FW_OW_Reset(ow);
    FW_OW_WaitACK(ow);
    DS_Write(ow, DS_ROM_SKIP);
    DS_Write(ow, DS_CMD_CONV);
//    FW_Delay_Us(50);
    
//    if(FW_OW_WaitACK(ow) != True)  return Sensor_RS_NoData;
    
//    if(FW_OW_Init(ow) != True)  return Sensor_RS_NoDev;
    FW_OW_Reset(ow);
    FW_OW_WaitACK(ow);
    DS_Write(ow, DS_ROM_SKIP);
    DS_Write(ow, DS_CMD_READ_REG);     //读取温度寄存器，共9个寄存器，前两个就是温度
    
    temp_l = DS_Read(ow);
    temp_h = DS_Read(ow);
    
//    for(i = 0; i < 9; i++)
//    {
//        msg[i] = DS_Read(ow);
//    }
//    
//    temp_l = msg[0];
//    temp_h = msg[1];
    
    temp = (temp_h << 8) | temp_l;
    
    if(temp & 0x8000)
    {
        temp = ~(temp - 1);
    }
    
    temp_int = (temp >> 4) * 10000;
    temp_f = (temp & 0x0F) * (acc == DS18B20_ACC_0_0625 ? 625 :
                              acc == DS18B20_ACC_0_125 ? 1250 :
                              acc == DS18B20_ACC_0_25 ? 2500 :
                              acc == DS18B20_ACC_0_5 ? 5000 :
                              625);
    
    if(temp & 0x8000)
        *pdata = (s32)(-(temp_int + temp_f));
    else
        *pdata = (temp_int + temp_f);
    
    return Sensor_RS_OK;
}





__CONST_STATIC_ Sensor_Driver_Type DS18B20_Driver =
{
    .Init           = DS_Init,
    .Reset          = DS_Reset,
    .Set_Period     = DS_Set_Period,
    .Get_Temprature = DS_Get_Temprature,
};
FW_DRIVER_REGIST("ds18b20->sensor", &DS18B20_Driver, DS18B20);





#include "fw_debug.h"
#if MODULE_TEST && SENSOR_TEST && DS18B20_TEST



static FW_OW_Type OW_Bus;
static void OW_Bus_Config(void *dev)
{
//    ((FW_OW_Type *)dev)->Pin = PE14;
    ((FW_OW_Type *)dev)->Pin = PB13;
}
FW_DEVICE_STATIC_REGIST("ow0", &OW_Bus, OW_Bus_Config, OW0);


static DS18B20_Type DS18B20;
static void DS18B20_Config(void *dev)
{
    DS18B20_Type *ds = dev;
    
    ds->Accuracy = DS18B20_ACC_0_0625;
    
    FW_Device_SetParent(ds, FW_Device_Find("ow0"));
    FW_Device_SetDriver(ds, FW_Driver_Find("ow->dev"));
}
FW_DEVICE_STATIC_REGIST("ds18b20", &DS18B20, DS18B20_Config, DS18B20);


static Sensor_Type Sensor;
static void Sensor_Config(void *dev)
{
    FW_Device_SetParent(dev, FW_Device_Find("ds18b20"));
    FW_Device_SetDriver(dev, FW_Driver_Find("ds18b20->sensor"));
}
FW_DEVICE_STATIC_REGIST("sensor", &Sensor, Sensor_Config, Sensor);





#define MAX      10
s32 Pressure = 0;
s32 Temprature = 0;
static s32 P_Buf[MAX];

void Test(void)
{
    Sensor_Type *sensor;
    s32 temp, sum = 0;
    u8 state, i, cnt = 0;
    
    
    sensor = FW_Device_Find("sensor");
    if(sensor == NULL)
    {
        while(1);
    }
    
    Sensor_Init(sensor);
    
    while(1)
    {
        state = Sensor_Get_Temprature(sensor, &temp);
        if(state != Sensor_RS_OK)
        {
            continue;
        }
    
        P_Buf[cnt++] = temp;
        if(cnt >= 10)
        {
            cnt = 0;
        }
        
        sum = 0;
        for(i = 0; i < MAX; i++)
        {
            sum += P_Buf[i];
        }
        
        Temprature = sum / MAX;
        
        FW_Delay_Ms(100);
    }
}


#endif

