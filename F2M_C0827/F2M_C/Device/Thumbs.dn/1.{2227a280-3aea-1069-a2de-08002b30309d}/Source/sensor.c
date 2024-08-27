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







