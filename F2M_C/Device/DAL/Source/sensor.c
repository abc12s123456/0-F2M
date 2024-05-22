#include "sensor.h"


void Sensor_Init(Sensor_Type *dev)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

void Sensor_Reset(Sensor_Type *dev)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Reset(dev);
}

void Sensor_Set_Period(Sensor_Type *dev, u16 period)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Set_Period(dev, period);
}

u8   Sensor_Get_Temprature(Sensor_Type *dev, s32 *pdata)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_Temprature(dev, pdata);
}

u8   Sensor_Get_Sample(Sensor_Type *dev, s32 *pdata)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_Sample(dev, pdata);
}

s32  Sensor_Formula(Sensor_Type *dev, s32 value)
{
    Sensor_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Formula(value);
}







