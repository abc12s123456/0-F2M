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
 
#include "key.h"

#include "fw_delay.h"
#include "fw_gpio.h"


#define KEY_DEBOUNCE_TIME         (100)         //消抖延时, 单位：ms


static Bool Key_Debounce_Delay(Key_Type *dev, u8 times);
static void Key_Debounce_Reset(Key_Type *dev);
static Bool Key_Equal_Delay(Key_Type *dev, u8 times);
static Bool Key_Unequal_Delay(Key_Type *dev, u8 times);

static Bool Key_Changed(Key_Type *dev);
static void Key_Record(Key_Type *dev);

static void Pin_Init(Key_Type *dev);
static u8   Pin_Read(Key_Type *dev);


void Key_DeInit(Key_Type *dev)
{
    dev->Init = NULL;
    dev->Read = NULL;
}

void Key_Init(Key_Type *dev)
{
    if(dev->Init == NULL || dev->Read == NULL)
    {
        dev->Init = Pin_Init;
        dev->Read = Pin_Read;
    }
    
    dev->Init(dev);
    
    if(dev->Read(dev) == dev->Level)
    {
        dev->Cur_Value = 1;
        dev->Pre_Value = 1;
        dev->Internal_State = Key_State_Press;
    }
    else
    {
        dev->Cur_Value = 0;
        dev->Pre_Value = 0;
        dev->Internal_State = Key_State_Idle;
    }
    
    dev->Long_Flag = False;
    dev->Double_Flag = False;
    
    dev->Export_State = Key_State_Idle;
}

u8 Key_Get(Key_Type *dev)
{
    u8 state = Key_State_Idle;
    
    if(dev->Read(dev) == dev->Level)
    {
        dev->Cur_Value = 1;
    }
    else
    {
        dev->Cur_Value = 0;
    }
    
    switch(dev->Internal_State)
    {
        case Key_State_Idle:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_Short;
                    Key_Record(dev);
                }
            }
            else
            {
                Key_Debounce_Reset(dev);
            }
            break;
            
        case Key_State_Release:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_Short;
                    Key_Record(dev);
                }
            }
            else
            {
                if(Key_Equal_Delay(dev, 2) == True)
                {
                    dev->Internal_State = Key_State_Idle;
                    state = Key_State_Idle;
                }
            }
            break;
            
        case Key_State_Short:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_Double;
                    Key_Record(dev);
                }
            }
            else
            {
                if(dev->Export_State != Key_State_Press)
                {
                    state = Key_State_Press;
                    dev->Export_State = state;
                }
                
                if(Key_Equal_Delay(dev, 10) == True)
                {
                    dev->Internal_State = Key_State_Long;
                    Key_Record(dev);
                }
            }
            break;
            
        case Key_State_Press:
            break;
        
        case Key_State_Double:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_KeepPress;
                    state = Key_State_Double;
                    dev->Double_Flag = True;
                    Key_Record(dev);
                }
            }
            else
            {
                if(Key_Equal_Delay(dev, 2) == True)
                {
                    dev->Internal_State = Key_State_Idle;
                    state = Key_State_Short;
                    dev->Short_Flag = True;
                }
            }
            break;
            
        case Key_State_Long:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_Idle;
                    Key_Record(dev);
                }
            }
            else
            {
                if(dev->Export_State == Key_State_Press)
                {
                    state = Key_State_Press;
                    dev->Export_State = Key_State_Idle;
                }
                
                if(Key_Equal_Delay(dev, 5) == True)
                {
                    dev->Internal_State = Key_State_KeepPress;
                    state = Key_State_Long;
                    dev->Long_Flag = True;
                }
            }
            break;
            
        case Key_State_KeepPress:
            if(Key_Changed(dev) == True)
            {
                if(Key_Unequal_Delay(dev, 1) == True)
                {
                    dev->Internal_State = Key_State_Release;
                    state = Key_State_Release;
                    Key_Record(dev);
                }
            }
            else
            {
                if(Key_Equal_Delay(dev, 5) == True)
                {
                    dev->Internal_State = Key_State_KeepPress;
                    state = Key_State_KeepPress;
                    dev->LPress_Flag = True;
                    Key_Debounce_Reset(dev);
                }
            }
            break;
            
        default:
            break;
    }
    
    dev->Export_State = state;
    
    return state;
}

u8   Key_Async_Get(Key_Type *dev)
{
    u8 state;
    
    if(dev->Short_Flag)  state = Key_State_Short;
    else if(dev->Double_Flag)  state = Key_State_Double;
    else if(dev->Long_Flag)  state = Key_State_Long;
    else  state = Key_State_Idle;
    
    dev->Short_Flag = False;
    dev->Double_Flag = False;
    dev->Long_Flag = False;
    
    return state;
}

u8   Key_Read_Level(Key_Type *dev)
{
    return dev->Read(dev);
}

/**
@功能：按键消抖延时
@参数：dev, 按键对象
       times, 消抖延时次数
@返回：True, 消抖延时到
       False, 消抖延时未到
@说明：无
*/
static Bool Key_Debounce_Delay(Key_Type *dev, u8 times)
{
    if(dev->Debounce_Flag == False)
    {
        /* 开始执行消抖计时 */
        dev->Start_Time = FW_Delay_GetMsStart();
        dev->Debounce_Flag = True;
    }
    else
    {
        u32 duration = FW_Delay_GetMsDuration(dev->Start_Time);
        
        /* 消抖延时到 */
        if(duration >= KEY_DEBOUNCE_TIME * times)
        {
            dev->Debounce_Flag = False;
            return True;
        }
    }
    
    return False;
}
/**/

/**
@功能：按键消抖延时重新计时
@参数：dev, 按键对象
@返回：无
@说明：无
*/
static void Key_Debounce_Reset(Key_Type *dev)
{
    dev->Debounce_Flag = False;
}
/**/

/**
@功能：按键未变化时消抖延时
@参数：dev, 按键对象
       times, 延时次数
@返回：True, 消抖延时到
       False, 消抖延时未到
@说明：无
*/
static Bool Key_Equal_Delay(Key_Type *dev, u8 times)
{
    if(dev->Equal_Flag == True)
    {
        Key_Debounce_Reset(dev);
        dev->Equal_Flag = False;
    }
    
    dev->Unequal_Flag = True;
    
    return Key_Debounce_Delay(dev, times);
}
/**/

/**
@功能：按键变化时消抖延时
@参数：dev, 按键对象
       times, 延时次数
@返回：True, 消抖延时到
       False, 消抖延时未到
@说明：无
*/
static Bool Key_Unequal_Delay(Key_Type *dev, u8 times)
{
    if(dev->Unequal_Flag == True)
    {
        Key_Debounce_Reset(dev);
        dev->Unequal_Flag = False;
    }
    
    dev->Equal_Flag = True;
    
    return Key_Debounce_Delay(dev, times);
}
/**/

static Bool Key_Changed(Key_Type *dev)
{
    return (Bool)(dev->Cur_Value != dev->Pre_Value);
}
/**/

static void Key_Record(Key_Type *dev)
{
    dev->Pre_Value = dev->Cur_Value;
}
/**/

static void Pin_Init(Key_Type *dev)
{
    if(dev->Level == LEVEL_H)
        FW_GPIO_Init(dev->Pin, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
    else
        FW_GPIO_Init(dev->Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
}
/**/

static u8   Pin_Read(Key_Type *dev)
{
    return FW_GPIO_Read(dev->Pin);
}
/**/


#include "project_debug.h"
#if MODULE_TEST && KEY_TEST
#include "fw_system.h"
#include "fw_uart.h"

#include "led.h"



void Test(void)
{
    u8 state;
    
//    LED_Type LED_G, LED_R;
    Key_Type K3;
    
    u8 cnt = 0;
    
    FW_System_Init();
    FW_Delay_Init();

    Key_DeInit(&K3);
    K3.Pin = PA0;
    K3.Level = LEVEL_L;
    Key_Init(&K3);
    
//    LED_DeInit(&LED_G);
//    LED_G.Pin = PD2;
//    LED_G.Level = LEVEL_L;
//    LED_Init(&LED_G);
//    
//    LED_DeInit(&LED_R);
//    LED_R.Pin = PD3;
//    LED_R.Level = LEVEL_L;
//    LED_Init(&LED_R);
    
    FW_UART_PrintInit(PB13, 9600);
    
    while(1)
    {
        state = Key_Get(&K3);
        
        if(state == Key_State_Short)
        {
            printf("短按\r\n");
//            LED_Shine(&LED_R, 500, 2);
        }
        else if(state == Key_State_Double)
        {
            printf("双击\r\n");
//            LED_Shine(&LED_G, 500, 2);
        }
        else if(state == Key_State_Long)
        {
            printf("长按\r\n");
//            LED_Breath(&LED_G, 1000);
        }
        else if(state == Key_State_KeepPress)
        {
            printf("按下\r\n");
//            LED_Breath(&LED_R, 1000);
        }
        else
        {
            if(cnt++ >= 100)
            {
                cnt = 0;
                printf("按键状态=%d\r\n", state);
            }
        }
        
        FW_Delay_Ms(10);
    }
}

#endif

