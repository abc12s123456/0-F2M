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
            FW_GPIO_Write(dev->SER_Pin, value & 0x80);
            value <<= 1;
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


/* SPI驱动, CS->RCK, SCK->SCK, MOSI->SER, OE、SCLR可由IO控制，也可直接有效电平 */
__INLINE_STATIC_ void SPI_HC595_Init(HC595_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    FW_SPI_Init(spi);
}

__INLINE_STATIC_ void SPI_HC595_CTL(HC595_Type *dev, u8 state)
{
    FW_GPIO_Write(dev->OE_Pin, state);
}

__INLINE_STATIC_ void SPI_HC595_Write(HC595_Type *dev, const u8 *pdata, u32 num)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Write(spi, 0, pdata, num);
    FW_SPI_CSSet(spi, Disable);
}




void HC595_Init(HC595_Type *dev)
{
    void *p, *q;
    
    p = FW_Device_GetDriver(dev);
    q = FW_Device_Find("spi->dev");
    if(p && p == q)
    {
        dev->Init = SPI_HC595_Init;
        dev->CTL = SPI_HC595_CTL;
        dev->Write = SPI_HC595_Write;
    }
    else
    {
        dev->Init = IO_HC595_Init;
        dev->CTL = IO_HC595_CTL;
        dev->Write = IO_HC595_Write;
    }
    
    dev->Init(dev);
}

void HC595_CTL(HC595_Type *dev, u8 state)
{
    dev->CTL(dev, state);
}

void HC595_Write(HC595_Type *dev, const u8 *pdata, u32 num)
{
    dev->Write(dev, pdata, num);
}




/* driver for gpio */
__INLINE_STATIC_ void HC595_Port_Init(FW_GPIO_Type *gpio, u16 port, FW_GPIO_Mode_Enum mode, FW_GPIO_Speed_Enum speed)
{
    HC595_Type *dev = FW_Device_GetParent(gpio);
    void *p, *q;
    
    p = FW_Device_GetDriver(dev);
    q = FW_Device_Find("spi->dev");
    if(p && p == q)
    {
        dev->Init = SPI_HC595_Init;
        dev->CTL = SPI_HC595_CTL;
        dev->Write = SPI_HC595_Write;
    }
    else
    {
        dev->Init = IO_HC595_Init;
        dev->CTL = IO_HC595_CTL;
        dev->Write = IO_HC595_Write;
    }
    
    dev->Init(dev);
}

/* one chip is one port, and each port has 8 pins */
__INLINE_STATIC_ void HC595_Port_Write(FW_GPIO_Type *gpio, u16 port, u32 value)
{
    HC595_Type *dev = FW_Device_GetParent(gpio);
    u8 num = PORTx(port);
    u8 *buffer = gpio->IO_Buffer;
    
    if(buffer == NULL)  return;
    
    buffer[gpio->Port_Num - num] = (u8)value;
    dev->Write(dev, buffer, gpio->Port_Num);
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
static u8 IO_Val[8];
static void EXT_GPIO_Config(void *dev)
{
    FW_GPIO_Type *gpio = dev;

    gpio->Pin_Num = 8;
    gpio->Port_Num = sizeof(IO_Val);
    gpio->IO_Buffer = IO_Val;
    
    FW_Device_SetParent(gpio, FW_Device_Find(HC595_DEV_NAME));
    FW_Device_SetDriver(gpio, FW_Driver_Find("hc595->gpio"));
}
FW_DEVICE_STATIC_REGIST(EXT_GPIO_NAME, &EXT_GPIO, EXT_GPIO_Config, EXT_GPIO);



void Test(void)
{
    FW_GPIO_Type *gpio = FW_Device_Find(EXT_GPIO_NAME);
    u16 port = PORT0;
    u8 *p = gpio->IO_Buffer;
    u8 i = 0;
    
    GPIO_PortInit(gpio, port, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    
    memset(gpio->IO_Buffer, 0xFF, gpio->Port_Num);
    
    while(1)
    {
//        GPIO_PortWrite(gpio, port, value++);
        GPIO_PortWrite(gpio, PORT0, p[7]);
        
        #if 1
        switch(i++)
        {
            case 0:
                p[7] = 0x00;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
                
            case 1:
                p[7] = 0x00;
                p[6] = 0x00;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
            
            case 2:
                p[7] = 0xFF;
                p[6] = 0x00;
                p[5] = 0x00;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
            
            case 3:
                p[7] = 0x00;
                p[6] = 0xFF;
                p[5] = 0x00;
                p[4] = 0x00;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
            
            case 4:
                p[7] = 0x00;
                p[6] = 0x00;
                p[5] = 0xFF;
                p[4] = 0x00;
                p[3] = 0x00;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
            
            case 5:
                p[7] = 0xFF;
                p[6] = 0x00;
                p[5] = 0x00;
                p[4] = 0xFF;
                p[3] = 0x00;
                p[2] = 0x00;
                p[1] = 0xFF;
                p[0] = 0xFF;
                break;
            
            case 6:
                p[7] = 0x00;
                p[6] = 0xFF;
                p[5] = 0x00;
                p[4] = 0x00;
                p[3] = 0xFF;
                p[2] = 0x00;
                p[1] = 0x00;
                p[0] = 0xFF;
                break;
            
            case 7:
                p[7] = 0x00;
                p[6] = 0x00;
                p[5] = 0xFF;
                p[4] = 0x00;
                p[3] = 0x00;
                p[2] = 0xFF;
                p[1] = 0x00;
                p[0] = 0x00;
                break;
            
            case 8:
                p[7] = 0xFF;
                p[6] = 0x00;
                p[5] = 0x00;
                p[4] = 0xFF;
                p[3] = 0x00;
                p[2] = 0x00;
                p[1] = 0xFF;
                p[0] = 0x00;
                break;
            
            case 9:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0x00;
                p[4] = 0x00;
                p[3] = 0xFF;
                p[2] = 0x00;
                p[1] = 0x00;
                p[0] = 0xFF;
                break;
            
            case 10:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0x00;
                p[3] = 0x00;
                p[2] = 0xFF;
                p[1] = 0x00;
                p[0] = 0x00;
                break;
            
            case 11:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0x00;
                p[2] = 0x00;
                p[1] = 0xFF;
                p[0] = 0x00;
                break;
            
            case 12:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0x00;
                p[1] = 0x00;
                p[0] = 0xFF;
                break;
            
            case 13:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0x00;
                p[0] = 0x00;
                break;
            
            case 14:
                p[7] = 0xFF;
                p[6] = 0xFF;
                p[5] = 0xFF;
                p[4] = 0xFF;
                p[3] = 0xFF;
                p[2] = 0xFF;
                p[1] = 0xFF;
                p[0] = 0x00;
                break;
            
            case 15:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
                
            default:
                break;
        }
        if(i >= 16)  i = 0;
        #else
        switch(i++)
        {
            case 0:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0x00;
                break;
                
            case 1:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0x00;
                p[7] = 0x00;
                break;
            
            case 2:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0x00;
                p[6] = 0x00;
                p[7] = 0xFF;
                break;
            
            case 3:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0x00;
                p[5] = 0x00;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            case 4:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0x00;
                p[4] = 0x00;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            case 5:
                p[0] = 0xFF;
                p[1] = 0xFF;
                p[2] = 0x00;
                p[3] = 0x00;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            case 6:
                p[0] = 0xFF;
                p[1] = 0x00;
                p[2] = 0x00;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            case 7:
                p[0] = 0x00;
                p[1] = 0x00;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            case 8:
                p[0] = 0x00;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 0xFF;
                p[4] = 0xFF;
                p[5] = 0xFF;
                p[6] = 0xFF;
                p[7] = 0xFF;
                break;
            
            default:
                break;
        }
        if(i >= 9)  i = 0;
        #endif
        
        
        FW_Delay_Ms(125);
    }
}


#endif

