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
#include "hc595.h"

#include "fw_gpio.h"
#include "fw_spi.h"
#include "fw_delay.h"

#include "fw_print.h"


/* IO驱动 */
__INLINE_STATIC_ void IO_HC595_Init(HC595_Type *dev)
{
    FW_GPIO_Init(dev->SER_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->OE_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->RCK_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->SCK_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->SCLR_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    FW_GPIO_CLR(dev->SER_Pin);
    FW_GPIO_CLR(dev->OE_Pin);
    FW_GPIO_CLR(dev->RCK_Pin);
    FW_GPIO_CLR(dev->SCK_Pin);
    FW_GPIO_SET(dev->SCLR_Pin);
}

__INLINE_STATIC_ void IO_HC595_CTL(HC595_Type *dev, u8 state)
{
    FW_GPIO_Write(dev->OE_Pin, state);
}

__INLINE_STATIC_ void IO_HC595_Write(HC595_Type *dev, const u8 *pdata, u32 num)
{
    u8 i, value;
    
    while(num--)
    {
        value = *pdata++;
        
        for(i = 0; i < 8; i++)
        {
            FW_GPIO_Write(dev->SER_Pin, value & 0x01);
            value >>= 1;
            FW_GPIO_CLR(dev->SCK_Pin);
            FW_Delay_Us(1);
            FW_GPIO_SET(dev->SCK_Pin);
            FW_Delay_Us(1);
        }
    }
    
    FW_GPIO_CLR(dev->RCK_Pin);
    FW_Delay_Us(1);
    FW_GPIO_SET(dev->RCK_Pin);
}


/* SPI驱动 */
__INLINE_STATIC_ void SPI_HC595_Init(HC595_Type *dev)
{

}

__INLINE_STATIC_ void SPI_HC595_CTL(HC595_Type *dev, u8 state)
{

}

__INLINE_STATIC_ void SPI_HC595_Write(HC595_Type *dev, const u8 *pdata, u32 num)
{

}




/* GPIO */
__INLINE_STATIC_ void HC595_Port_Init(FW_GPIO_Type *gpio, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    HC595_Type *dev = FW_Device_GetParent(gpio);
    char *name = FW_Device_GetName(dev);
    void *p, *q;
    
    p = FW_Device_GetDriver(dev);
    q = FW_Device_Find("spi->dev");
    if(p && p == q)
    {
        dev->Init = SPI_HC595_Init;
        dev->CTL = SPI_HC595_CTL;
        dev->Write = SPI_HC595_Write;
        goto Init;
    }
    else
    {
        dev->Init = IO_HC595_Init;
        dev->CTL = IO_HC595_CTL;
        dev->Write = IO_HC595_Write;
        goto Init;
    }
    
    Init:
    dev->Init(dev);
}

/* one chip is one port, and each port has 8 pins */
__INLINE_STATIC_ void HC595_Port_Write(FW_GPIO_Type *gpio, u16 port, u32 value)
{
    HC595_Type *dev = FW_Device_GetParent(gpio);
    u8 num = PORTx(port);
    u8 *buffer = gpio->IO_Buffer;
    
    if(buffer == NULL)  return;
    
    buffer[num - 1] = (u8)value;
    dev->Write(dev, buffer, num);
}




__CONST_STATIC_ FW_GPIO_Driver_Type HC595_GPIO_Driver =
{
    .Port_Init = HC595_Port_Init,
    .Port_Write = HC595_Port_Write,
};
FW_DRIVER_REGIST("hc595->gpio", &HC595_GPIO_Driver, HC595_GPIO);




#include "fw_debug.h"
#if MODULE_TEST && (IO_DEVICE_TEST || SPI_DEVICE_TEST) && HC595_TEST


#define HC595_DEV_NAME       "hc595"
static HC595_Type HC595;
static void HC595_Config(void *dev)
{
    HC595_Type *hc595 = dev;
    
    hc595->SER_Pin = PA7;
    hc595->RCK_Pin = PA6;
    hc595->SCK_Pin = PA5;
//    hc595->OE_Pin = PA4;
//    hc595->SCLR_Pin = PA3;
}
FW_DEVICE_STATIC_REGIST(HC595_DEV_NAME, &HC595, HC595_Config, HC595);


#define EXT_GPIO_NAME        "ex_gpio"
static FW_GPIO_Type EXT_GPIO;
static u8 IO_Val[5];
static void EXT_GPIO_Config(void *dev)
{
    FW_GPIO_Type *gpio = dev;

    gpio->Pin_Num = 8;
    gpio->Port_Num = 5;
    gpio->IO_Buffer = IO_Val;
    
    FW_Device_SetParent(gpio, FW_Device_Find(HC595_DEV_NAME));
    FW_Device_SetDriver(gpio, FW_Driver_Find("hc595->gpio"));
}
FW_DEVICE_STATIC_REGIST(EXT_GPIO_NAME, &EXT_GPIO, EXT_GPIO_Config, EXT_GPIO);



void Test(void)
{
    FW_GPIO_Type *gpio = FW_Device_Find(EXT_GPIO_NAME);
    u16 port = PORT0;
    u8 value = 0;
    
    GPIO_PortInit(gpio, port, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    
    while(1)
    {
        GPIO_PortWrite(gpio, port, value++);
        FW_Delay_Ms(500);
    }
}


#endif

