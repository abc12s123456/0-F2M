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
 
#include "io_encoder.h"

#include "qencoder.h"

#include "fw_gpio.h"

#include "fw_debug.h"
#include "fw_print.h"


/*
1、使用普通IO口进行驱动；
2、调用周期<30ms，旋转速度不宜过快，避免出现跳码；
*/


#define COUNTER_DEF_MAX  0xFF


__INLINE_STATIC_ void IO_Encoder_Init(QEncoder_Type *dev)
{
    QEncoder_Config_Type *cfg = &dev->Config;
    
    if(cfg->VL_A == LEVEL_H)
    {
        FW_GPIO_Init(cfg->Pin_A, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
    }
    else
    {
        FW_GPIO_Init(cfg->Pin_A, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    }
    
    if(cfg->VL_B == LEVEL_H)
    {
        FW_GPIO_Init(cfg->Pin_B, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
    }
    else
    {
        FW_GPIO_Init(cfg->Pin_B, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    }

    dev->State = 0;
    dev->Counter = 0;
    
    if(cfg->Max == 0)  cfg->Max = COUNTER_DEF_MAX;
}

__INLINE_STATIC_ u32  IO_Encoder_GetCounter(QEncoder_Type *dev)
{
    QEncoder_Config_Type *cfg = &dev->Config;
    
    if(FW_GPIO_Read(cfg->Pin_A) == cfg->VL_A)
    {
        dev->State |= 0x02;
    }
    
    if(FW_GPIO_Read(cfg->Pin_B) == cfg->VL_B)
    {
        dev->State |= 0x01;
    }
    
    switch(dev->State)
    {
        case 0x01:
        case 0x07:
        case 0x08:
        case 0x0E:
            dev->Counter -= 1;
            break;
        
        case 0x02:
        case 0x04:
        case 0x0B:
        case 0x0D:
            dev->Counter += 1;
            break;
        
        /* 容易产生跳值 */
//        case 0x03:
//        case 0x0C:
//            dev->Counter -= 2;
//            break;
//        
//        case 0x06:
//        case 0x09:
//            dev->Counter += 2;
//            break;
        
        default:
            break;
    }
    
    dev->State <<= 2;
    dev->State &= 0x0C;
    
    if((s32)dev->Counter > (s32)cfg->Max)
    {
        dev->Counter = 0;
    }
    else if((s32)dev->Counter < 0)
    {
        dev->Counter = cfg->Max;
    }
    else
    {}
    
    return dev->Counter;
}




/**
 * @ Encoder Driver
 */
__CONST_STATIC_ QEncoder_Driver_Type IO_Encoder_Driver =
{
    .Init = IO_Encoder_Init,
    .Get_Counter = IO_Encoder_GetCounter,
};
FW_DRIVER_REGIST("io->encoder", &IO_Encoder_Driver, IO_Encoder);




#include "project_debug.h"
#if MODULE_TEST && IO_DEVICE_TEST && QENCODER_TEST
#include "fw_system.h"
#include "fw_delay.h"

#include "fw_timer.h"
#include "fw_led.h"

#include "qencoder.h"



//#define QENCODER_DEV_ID      FW_DEVICE_ID(FW_DT_Encoder, QENCODER_IO_DRV_NUM, NULL, NULL)
#define QENCODER_DEV_ID      FW_DEVICE_ID(FW_DT_Encoder, QENCODER_TIM_DRV_NUM, NULL, NULL)

#define TIM_DEV_ID           FW_DEVICE_ID(FW_DT_Timer, TIMER_NATIVE_DRV_NUM, 2, NULL)


static QEncoder_Type QEncoder;

static void QEncoder_Config(void *dev)
{
    QEncoder.Pin_A = PC6;
    QEncoder.Pin_B = PC7;
    
    QEncoder.CH_A = FW_TIM_CH0;
    QEncoder.CH_B = FW_TIM_CH1;
    
    QEncoder.TL_A = LEVEL_L;
    QEncoder.TL_B = LEVEL_L;
    
    QEncoder.Parent.Device = FW_Device_Find(TIM_DEV_ID);
}
FW_DEVICE_STATIC_REGIST(QENCODER_DEV_ID, &QEncoder, QEncoder_Config, QEncoder);


u32 counter;


void Test(void)
{
    QEncoder_Type *qencoder;
    s32 ev;
    
    FW_System_Init();
    FW_Delay_Init();
    
    u16 VCC_EN = PD1;
    FW_LED_Type LED;
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    LED.Pin = PB4;
    LED.Level = LEVEL_L;
    FW_LED_Init(&LED);
    
    FW_GPIO_SET(VCC_EN);
    
    while(1)
    {
        FW_LED_Breath(&LED, 1000);
    }
    
    qencoder = FW_Device_Find(QENCODER_DEV_ID);
    if(qencoder == NULL)
    {
        while(1);
    }
    
    QEncoder_Init(qencoder);
    
    while(1)
    {
        ev = QEncoder_Get(qencoder);
        if(ev)
        {
            counter += ev;
        }
        
        FW_Delay_Ms(10);
    }
}


#endif

