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
#include "fw_timer.h"

#include "fw_debug.h"


void FW_TIM_SetPort(FW_TIM_Type *dev, void *timer)
{
    if(dev->TIMx == NULL)  dev->TIMx = timer;
}
/**/

void *FW_TIM_GetPort(FW_TIM_Type *dev)
{
    return dev->TIMx;
}
/**/

u32  FW_TIM_GetUnit(FW_TIM_Type *dev)
{
    return (dev->Unit == FW_TIM_Unit_Us) ? 1000000 :\
           (dev->Unit == FW_TIM_Unit_Ms) ? 1000 :\
           (dev->Unit == FW_TIM_Unit_Ns) ? 1000000000 :\
           1000000;
}
/**/

void FW_TIM_SetFrequency(FW_TIM_Type *dev, u32 freq)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Set_Frequency(dev, freq);
}
/**/

u32  FW_TIM_GetFrequency(FW_TIM_Type *dev)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_Frequency(dev);
}
/**/

u32  FW_TIM_GetClock(FW_TIM_Type *dev)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    dev->Clock = drv->Get_Clock(dev);
    return dev->Clock;
}
/**/
    
void FW_TIM_Init(FW_TIM_Type *dev)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    dev->Clock = drv->Get_Clock(dev);
    drv->Timing_Init(dev);
    
    (dev->IT_EN) ? drv->IT_CTL(dev, Enable) : drv->IT_CTL(dev, Disable);
    (dev->EN) ? drv->CTL(dev, Enable) : drv->CTL(dev, Disable);
}
/**/

void FW_TIM_CTL(FW_TIM_Type *dev, u8 state)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->CTL(dev, state);
}
/**/

void FW_TIM_ITCTL(FW_TIM_Type *dev, u8 state)
{
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->IT_CTL(dev, state);
}
/**/

void FW_TIM_BindCB(FW_TIM_Type *dev, void (*cb)(void *), void *pdata)
{
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
}
/**/

void FW_TIM_IH(FW_TIM_Type *dev)
{
    if(dev->IH_CB)  dev->IH_CB(dev->IH_Pdata);
}
/**/




#include "pwm.h"

__INLINE_STATIC_ void PWM_Init(PWM_Type *dev)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    drv->PWM_Init(timer, &dev->Config);
}
/**/

__INLINE_STATIC_ void PWM_Set_Frequency(PWM_Type *dev, u32 freq)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    /* PWM的频率越高，占空比越无法精确调整 */
    if(freq > timer->Clock)  freq = timer->Clock;
    
    drv->Set_Frequency(timer, freq);
}
/**/

__INLINE_STATIC_ u32  PWM_Get_Frequency(PWM_Type *dev)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    return drv->Get_Frequency(timer);
}
/**/

__INLINE_STATIC_ void PWM_Set_DC(PWM_Type *dev, u16 dc)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    if(dc > PWM_DC_MAX)  dc = PWM_DC_MAX;
    
    drv->Set_DC(timer, dev->Config.CH_P, dc);
}
/**/

__INLINE_STATIC_ u16  PWM_Get_DC(PWM_Type *dev)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    return drv->Get_DC(timer, dev->Config.CH_P);
}
/**/

__INLINE_STATIC_ void PWM_CTL(PWM_Type *dev, u8 state)
{
    FW_TIM_Type *timer = FW_Device_GetParent(dev);
    FW_TIM_Driver_Type *drv = FW_Device_GetDriver(timer);
    
    drv->PWM_CTL(timer, dev->Config.CH_P, state);
}
/**/




/* timer pwm driver */
__CONST_STATIC_ PWM_Driver_Type PWM_Driver =
{
    .Init          = PWM_Init,
    .Set_Frequency = PWM_Set_Frequency,
    .Get_Frequency = PWM_Get_Frequency,
    .Set_DC        = PWM_Set_DC,
    .Get_DC        = PWM_Get_DC,
    .CTL           = PWM_CTL,
};
FW_DRIVER_REGIST("timer->pwm", &PWM_Driver, PWM_TIM);




#include "fw_debug.h"
#if MODULE_TEST && PWM_TEST && TIMER_TEST
#include "fw_gpio.h"
#include "fw_delay.h"


#define HTIM_NAME        "timer4"
static void HTIM_Pre_Init(void *pdata)
{
    FW_TIM_Type *timer = FW_Device_Find(HTIM_NAME);
    timer->EN = ON;
    timer->Mode = FW_TIM_Mode_PWM;
}
FW_PRE_INIT(HTIM_Pre_Init, NULL);


#define PWM_NAME         "pwm"
static PWM_Type PWM;
static void PWM_Config(void *dev)
{
    PWM_Type *pwm = dev;
    PWM_Config_Type *config = &pwm->Config;
    
    config->Pin_P = PA1;
    config->CH_P = FW_TIM_CH1;
    config->Frequency = 1000;
    config->DC = 9000;
    config->IT_EN = ON;
    
    FW_Device_SetParent(dev, FW_Device_Find(HTIM_NAME));
    FW_Device_SetDriver(dev, FW_Driver_Find("timer->pwm"));
}
FW_DEVICE_STATIC_REGIST(PWM_NAME, &PWM, PWM_Config, PWM0);


void Test(void)
{
    u16 VCC_EN = PC13;
    
    PWM_Type *pwm;
    
    pwm = FW_Device_Find(PWM_NAME);
    if(pwm == NULL)  while(1);
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    PWM_Init(pwm);
    
    PWM_CTL(pwm, Enable);
    
    while(1);
}


#endif
