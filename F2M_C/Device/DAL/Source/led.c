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
#include "led.h"

#include "fw_delay.h"
#include "fw_gpio.h"


__INLINE_STATIC_ void Pin_Init(LED_Type *dev);
__INLINE_STATIC_ void Pin_CTL(LED_Type *dev, u8 state);


void LED_DeInit(LED_Type *dev)
{
    dev->Init = NULL;
    dev->CTL = NULL;
    dev->Timing = NULL;
}

void LED_Init(LED_Type *dev)
{
    if(dev->Init == NULL || dev->CTL == NULL)
    {
        dev->Init = Pin_Init;
        dev->CTL = Pin_CTL;
    }
    
    /* 软定时器 */
    if(dev->Timing)
    {
        if(dev->Timing->Get_Tick == NULL)
        {
            STiming_Init(dev->Timing, dev->Timing->Get_Tick);
        }
    }
    
    /* 初始化 */
    dev->Init(dev);
    
    /* 初始化完成后，默认关闭 */
    dev->CTL(dev, OFF);
    dev->State = OFF;
}

void LED_ON(LED_Type *dev)
{
    dev->CTL(dev, ON);
    dev->State = ON;
}

void LED_OFF(LED_Type *dev)
{
    dev->CTL(dev, OFF);
    dev->State = OFF;
}

void LED_Shine(LED_Type *dev, u16 period, u8 times)
{
    period >>= 1;
    while(times--)
    {
        LED_ON(dev);
        FW_Delay_Ms(period);
        LED_OFF(dev);
        FW_Delay_Ms(period);
    }
}

void LED_Blink(LED_Type *dev, u16 period)
{
    if(STiming_Start(dev->Timing, period >> 1) == True)
    {
        dev->State ? LED_OFF(dev) : LED_ON(dev);
    }
}

void LED_Adj_Blink(LED_Type *dev, u16 period, u8 dc)
{
    u16 tmp;
    
    if(dc > 100)  dc = 100;
    
    /* LED打开后，等待period * dc / 100后关闭 */
    if(dev->State)
    {
        tmp = period * dc / 100;
        if(STiming_Start(dev->Timing, tmp) == True)
        {
            LED_OFF(dev);
            STiming_Reset(dev->Timing);
        }
    }
    else
    {
        tmp = period * (100 - dc) / 100;
        if(STiming_Start(dev->Timing, tmp) == True)
        {
            LED_ON(dev);
            STiming_Reset(dev->Timing);
        }
    }
}

#define LED_BREATH_PERIOD    20        //呼吸周期，单位：ms

void LED_Breath(LED_Type *dev, u16 period)
{
    u16 times, offset;
    u16 i, j = 0;

    /* period >> 2的目的是一个呼吸周期为暗->亮->暗，而暗->亮及亮->暗的空档，还需
       要一个相同时间的延迟;
       j为占空比值 */
    period >>= 2;
    
    while((period) > (++j * LED_BREATH_PERIOD * LED_BREATH_PERIOD));
    times = j * LED_BREATH_PERIOD;
    offset = j;
    j = 0;
    
    /* 由暗至亮 */
    for(i = 1; i < times; i++)
    {
        dev->CTL(dev, OFF);
        FW_Delay_Ms(LED_BREATH_PERIOD - j);
        dev->CTL(dev, ON);
        FW_Delay_Ms(j);
        if((i % offset) == 0)  j++;
    }
    
    /* 控制呼吸灯的周期 */
    FW_Delay_Ms(period);
    
    /* 由亮至暗 */
    j = 0;
    for(i = 1; i < times; i++)
    {
        dev->CTL(dev, ON);
        FW_Delay_Ms(LED_BREATH_PERIOD - j);
        dev->CTL(dev, OFF);
        FW_Delay_Ms(j);
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
}

void LED_Exq_Breath(LED_Type *dev, u16 period)
{
    u32 times, offset;
    u32 i, j = 0, k;
    
    /* period >> 2的目的是一个呼吸周期为暗->亮->暗，而暗->亮及亮->暗的空档，还需
       要一个相同时间的延迟;
       j为占空比值 */
    period >>= 2;
    
    while((period) > (++j * LED_BREATH_PERIOD * LED_BREATH_PERIOD));
    times = j * LED_BREATH_PERIOD;
    offset = j;
    j = 0;
    
    for(i = 1; i < times; i++)
    {
        for(k = 0; k < 900; k++)
        {
            dev->CTL(dev, OFF);
            FW_Delay_Us(LED_BREATH_PERIOD - j);
            dev->CTL(dev, ON);
            FW_Delay_Us(j);
        }
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
    
    j = 0;
    for(i = 0; i < times; i++)
    {
        for(k = 0; k < 900; k++)
        {
            dev->CTL(dev, ON);
            FW_Delay_Us(LED_BREATH_PERIOD - j);
            dev->CTL(dev, OFF);
            FW_Delay_Us(j);
        }
        if((i % offset) == 0)  j++;
    }
    
    FW_Delay_Ms(period);
}

__INLINE_STATIC_ void Pin_Init(LED_Type *dev)
{
    if(dev->Level)
        FW_GPIO_Init(dev->Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    else
        FW_GPIO_Init(dev->Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
}

__INLINE_STATIC_ void Pin_CTL(LED_Type *dev, u8 state)
{
    if(state)
        FW_GPIO_Write(dev->Pin, dev->Level);
    else
        FW_GPIO_Write(dev->Pin, !dev->Level);
}

