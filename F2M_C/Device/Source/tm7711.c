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
#include "tm7711.h"

#include "fw_gpio.h"
#include "fw_delay.h"

#include "sensor.h"


#define SCK_H(dev)           FW_GPIO_SET(dev->PD_SCK_Pin)
#define SCK_L(dev)           FW_GPIO_CLR(dev->PD_SCK_Pin)

#define DOUT_Read(dev)       FW_GPIO_Read(dev->DOUT_Pin)

#define SCK_SAMP_NUM         24
#define SCK_10HZ_NUM         25
#define SCK_TEMP_NUM         26
#define SCK_40HZ_NUM         27


typedef enum
{
    TM77_CH_10Hz = 1,
    TM77_CH_40Hz,
    TM77_CH_Temp,
}TM77_CH_Enum;


__INLINE_STATIC_ s32  Code_GetSrc(u32 code, u8 bit_num)
{
    s32 value = code;
    
    if(bit_num == 0)  return 0;
    
    if(code & (1 << (bit_num - 1)))                       //负数
    {
        value = code & (~(1 << (bit_num - 1)));           //符号位清0
        value = ~(value - 1) & ~(0xFFFFFFFF << bit_num);  //其余位-1后取反得到绝对值
        value |= 1 << (bit_num - 1);                      //添加符号位
    }
    return value;
}


__INLINE_STATIC_ void TM77_Init(Sensor_Type *sensor)
{
    TM77_Type *dev = FW_Device_GetParent(sensor);
    FW_GPIO_Init(dev->PD_SCK_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_High);
    FW_GPIO_Init(dev->DOUT_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_High);
}

__INLINE_STATIC_ void TM77_Reset(Sensor_Type *sensor)
{

}

__INLINE_STATIC_ void TM77_Set_Period(Sensor_Type *sensor, u16 period)
{
    TM77_Type *dev = FW_Device_GetParent(sensor);
    u8 i;
    
    if(period == 100)
    {
        if(dev->Channel != TM77_CH_10Hz)
        {
            dev->Channel = TM77_CH_10Hz;
            
            for(i = 0; i < SCK_10HZ_NUM; i++)
            {
                SCK_H(dev);
                FW_Delay_Us(1);
                SCK_L(dev);
                FW_Delay_Us(1);
            }
        }
    }
    else if(period == 25)
    {
        if(dev->Channel != TM77_CH_40Hz)
        {
            dev->Channel = TM77_CH_40Hz;
            
            for(i = 0; i < SCK_40HZ_NUM; i++)
            {
                SCK_H(dev);
                FW_Delay_Us(1);
                SCK_L(dev);
                FW_Delay_Us(1);
            }
        }
    }
    else
    {
        
    }
}

__INLINE_STATIC_ u8   TM77_Get_Temprature(Sensor_Type *sensor, s32 *pdata)
{
    TM77_Type *dev = FW_Device_GetParent(sensor);
    s32 value = 0;
    u8 i;
    
    if(DOUT_Read(dev) == LEVEL_H)  return Sensor_RS_NoData;
    
    if(dev->Channel != TM77_CH_Temp)
    {
        dev->Channel = TM77_CH_Temp;
        
        for(i = 0; i < SCK_TEMP_NUM; i++)
        {
            SCK_H(dev);
            FW_Delay_Us(1);
            SCK_L(dev);
            FW_Delay_Us(1);
        }
        
        return Sensor_RS_NoData;
    }
    else
    {
        for(i = 0; i < SCK_SAMP_NUM; i++)
        {
            SCK_H(dev);
            value <<= 1;
            FW_Delay_Us(5);
            if(DOUT_Read(dev) == LEVEL_H)  value |= 1;
            SCK_L(dev);
        }
        
        SCK_H(dev);
        FW_Delay_Us(1);
        SCK_L(dev);
        FW_Delay_Us(1);
        SCK_H(dev);
        FW_Delay_Us(1);
        SCK_L(dev);
        
        *pdata = Code_GetSrc(value, SCK_SAMP_NUM);
        
        return Sensor_RS_OK;
    }
}

__INLINE_STATIC_ u8   TM77_Get_Sample(Sensor_Type *sensor, s32 *pdata)
{
    TM77_Type *dev = FW_Device_GetParent(sensor);
    s32 value = 0;
    u8 i;
    
    if(DOUT_Read(dev) == LEVEL_H)  return Sensor_RS_NoData;
    
    if(dev->Channel != TM77_CH_10Hz && dev->Channel != TM77_CH_40Hz)
    {
        dev->Channel = TM77_CH_10Hz;
        
        for(i = 0; i < SCK_10HZ_NUM; i++)
        {
            SCK_H(dev);
            FW_Delay_Us(1);
            SCK_L(dev);
            FW_Delay_Us(1);
        }
        
        return Sensor_RS_NoData;
    }
    else
    {
        for(i = 0; i < SCK_SAMP_NUM; i++)
        {
            SCK_H(dev);
            value <<= 1;
            FW_Delay_Us(5);
            if(DOUT_Read(dev) == LEVEL_H)  value |= 1;
            SCK_L(dev);
        }
        
        SCK_H(dev);
        FW_Delay_Us(1);
        SCK_L(dev);
        FW_Delay_Us(1);
        
        if(dev->Channel == TM77_CH_40Hz)
        {
            SCK_H(dev);
            FW_Delay_Us(1);
            SCK_L(dev);
            FW_Delay_Us(1);
            SCK_H(dev);
            FW_Delay_Us(1);
            SCK_L(dev);
            FW_Delay_Us(1);
        }
        
        *pdata = Code_GetSrc(value, SCK_SAMP_NUM);
        
        return Sensor_RS_OK;
    }
}

__INLINE_STATIC_ s32  TM77_Sample_Formula(s32 value)
{
    return value;
}




__CONST_STATIC_ Sensor_Driver_Type TM77_ADC_Driver =
{
    .Init = TM77_Init,
    
    .Reset = TM77_Reset,
    .Set_Period = TM77_Set_Period,
    
    .Get_Temprature = TM77_Get_Temprature,
    .Get_Sample = TM77_Get_Sample,
    .Formula = TM77_Sample_Formula,
};
FW_DRIVER_REGIST("tm77->sensor", &TM77_ADC_Driver, TM7711);




#include "fw_debug.h"
#if MODULE_TEST && SENSOR_TEST && TM7711_TEST
#include "fw_uart.h"


#define TM77_DEV_NAME        "tm7711"
static  TM77_Type            TM7711;
static  void TM77_Config(void *dev)
{
    TM77_Type *tm = dev;
    tm->PD_SCK_Pin = PB8;
    tm->DOUT_Pin = PB9;
}
FW_DEVICE_STATIC_REGIST(TM77_DEV_NAME, &TM7711, TM77_Config, TM7711);


#define SENSOR_DEV_NAME      "sensor"
static  Sensor_Type          Sensor;
static  void Sensor_Config(void *dev)
{
    FW_Device_SetParent(dev, FW_Device_Find(TM77_DEV_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("tm77->sensor"));
}
FW_DEVICE_STATIC_REGIST(SENSOR_DEV_NAME, &Sensor, Sensor_Config, Sensor_Gravity);



#define NUM   10


static s32 Buffer[NUM];


void Test(void)
{
    u16 VCC_EN = PB5;
    Sensor_Type *sensor;
    u8 state;
    s32 value;
    s32 sum = 0;
    u8 cnt = 0;
    u8 i;
    
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    sensor = FW_Device_Find(SENSOR_DEV_NAME);
    while(!sensor);
    Sensor_Init(sensor);
    
    FW_UART_PrintInit(PD4, 9600);
    printf("hellor world\r\n");
    
    while(1)
    {
        state = Sensor_Get_Sample(sensor, &value);
        if(state != Sensor_RS_OK)
        {
            continue;
        }
        
        Buffer[cnt] = value;
        cnt++;
        if(cnt >= NUM)  cnt = 0;
        
        for(i = 0; i < NUM; i++)
        {
            sum += Buffer[cnt];
        }
        
        value = sum / NUM;
        sum = 0;
        
        printf("%d\r\n", value);
        
        FW_Delay_Ms(100);
    }
}


#endif

