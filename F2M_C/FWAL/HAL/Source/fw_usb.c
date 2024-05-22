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
 
#include "fw_usb.h"
#include "fw_gpio.h"
#include "fw_delay.h"


void FW_USB_Init(FW_USB_Type *dev)
{
    FW_USB_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(dev->EN_Pin)
    {
        if(dev->EN_VL == LEVEL_L)
        {
            FW_GPIO_Init(dev->EN_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            
            FW_GPIO_SET(dev->EN_Pin);
            FW_Delay_Ms(100);
            FW_GPIO_CLR(dev->EN_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->EN_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            
            FW_GPIO_CLR(dev->EN_Pin);
            FW_Delay_Ms(100);
            FW_GPIO_SET(dev->EN_Pin);
        }
        
        dev->Connect_Flag = True;
    }
    
    drv->Init(dev);
}








