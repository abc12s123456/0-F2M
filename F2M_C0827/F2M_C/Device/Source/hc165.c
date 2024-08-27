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
#include "hc165.h"

#include "fw_gpio.h"
#include "fw_spi.h"
#include "fw_delay.h"

#include "fw_print.h"


/* IO驱动 */
__INLINE_STATIC_ void IO_HC165_Init(HC165_Type *dev)
{
    FW_GPIO_Init(dev->PL_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->CLK_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->EN_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_Init(dev->Data_Pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);

    FW_GPIO_SET(dev->PL_Pin);
    FW_GPIO_SET(dev->CLK_Pin);
    FW_GPIO_SET(dev->EN_Pin);
}

__INLINE_STATIC_ void IO_HC165_CTL(HC165_Type *dev, u8 state)
{
    
}

__INLINE_STATIC_ u32  IO_HC165_Read(HC165_Type *dev, u8 *pdata, u32 num)
{
    u8 value;
    u8 i, j;
    
    FW_GPIO_SET(dev->EN_Pin);
    FW_GPIO_CLR(dev->PL_Pin);
    FW_Delay_Us(1);
    FW_GPIO_CLR(dev->EN_Pin);
    FW_GPIO_SET(dev->PL_Pin);
    
    for(i = 0; i < num; i++)
    {
        value = 0;
        
        for(j = 0; j < 8; j++)
        {
            value <<= 1;
            value |= FW_GPIO_Read(dev->Data_Pin);
            FW_GPIO_CLR(dev->CLK_Pin);
            FW_Delay_Us(1);
            FW_GPIO_SET(dev->CLK_Pin);
            FW_Delay_Us(1);
        }
        
        *pdata++ = value;
    }
    
    FW_GPIO_SET(dev->EN_Pin);
    
    return num;
}


/* SPI驱动 */
__INLINE_STATIC_ void SPI_HC165_Init(HC165_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_Init(spi);
}

__INLINE_STATIC_ void SPI_HC165_CTL(HC165_Type *dev, u8 state)
{
    
}

__INLINE_STATIC_ u32  SPI_HC165_Read(HC165_Type *dev, u8 *pdata, u32 num)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 value;
    
    FW_SPI_CSSet(spi, Disable);
    value = 0xFD;
    FW_SPI_Write(spi, 0, &value, 1);
    
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Read(spi, 0, pdata, num);
    FW_SPI_CSSet(spi, Disable);
    
    return num;
}




/* GPIO */
__INLINE_STATIC_ void HC165_Port_Init(FW_GPIO_Type *gpio, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    HC165_Type *dev = FW_Device_GetParent(gpio);
    void *p, *q;
    
    p = FW_Device_GetDriver(dev);
    q = FW_Device_Find("spi->dev");
    if(p && p == q)
    {
        dev->Init = SPI_HC165_Init;
        dev->CTL = SPI_HC165_CTL;
        dev->Read = SPI_HC165_Read;
    }
    else
    {
        dev->Init = IO_HC165_Init;
        dev->CTL = IO_HC165_CTL;
        dev->Read = IO_HC165_Read;
    }
    
    dev->Init(dev);
}

/* one chip is one port, and each port has 8 pins */
__INLINE_STATIC_ u32  HC165_Port_Read(FW_GPIO_Type *gpio, u16 port)
{
    HC165_Type *dev = FW_Device_GetParent(gpio);
    u8 num = PORTx(port);
    u8 *buffer = gpio->IO_Buffer;
    
    if(buffer == NULL)  return 0xFFFFFFFF;
    
    dev->Read(dev, buffer, num);
    
    return buffer[num - 1];
}




__CONST_STATIC_ FW_GPIO_Driver_Type HC165_GPIO_Driver =
{
    .Port_Init = HC165_Port_Init,
    .Port_Read = HC165_Port_Read,
};
FW_DRIVER_REGIST("hc165->gpio", &HC165_GPIO_Driver, HC165_GPIO);




#include "fw_debug.h"
#if MODULE_TEST && (IO_DEVICE_TEST || SPI_DEVICE_TEST) && HC165_TEST


#define HC165_DEV_NAME       "hc165"
static HC165_Type HC165;
static void HC165_Config(void *dev)
{
    HC165_Type *hc165 = dev;
    
    hc165->PL_Pin = PA3;
    hc165->CLK_Pin = PA5;
    hc165->Data_Pin = PA6;
    hc165->EN_Pin = PA7;
}
FW_DEVICE_STATIC_REGIST(HC165_DEV_NAME, &HC165, HC165_Config, HC165);


#define EX_GPIO_NAME         "ex_gpio"
static FW_GPIO_Type EX_GPIO;
static u8 IO_Val[5];
static void EX_GPIO_Config(void *dev)
{
    FW_GPIO_Type *gpio = dev;
    
    gpio->Pin_Num = 8;
    gpio->Port_Num = 5;
    gpio->IO_Buffer = IO_Val;
    
    FW_Device_SetParent(gpio, FW_Device_Find(HC165_DEV_NAME));
    FW_Device_SetDriver(gpio, FW_Driver_Find("hc165->gpio"));
}
FW_DEVICE_STATIC_REGIST(EX_GPIO_NAME, &EX_GPIO, EX_GPIO_Config, EX_GPIO);




void Test(void)
{

}


#endif

