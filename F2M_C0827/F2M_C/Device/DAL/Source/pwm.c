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
#include "pwm.h"




void PWM_Init(PWM_Type *dev)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

void PWM_Set_Frequency(PWM_Type *dev, u32 freq)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Set_Frequency(dev, freq);
}

void PWM_Set_DC(PWM_Type *dev, u16 dc)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    if(dc > PWM_DC_MAX)  dc = PWM_DC_MAX;
    drv->Set_DC(dev, dc);
}

void PWM_Set_PulseNum(PWM_Type *dev, u32 num)
{
    
}

void PWM_CTL(PWM_Type *dev, u8 state)
{
    PWM_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->CTL(dev, state);
}

