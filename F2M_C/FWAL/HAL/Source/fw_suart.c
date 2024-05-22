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
 
#include "fw_suart.h"
#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"

#include "libc.h"


/*
    波特率	延时（us） ±5%误差(us)
     1200	 833.333     41.667
     2400	 416.666     20.833
     4800	 208.333     10.417
     9600	 104.166     5.208
     14400	 69.444      3.472
     19200	 52.083      2.604   //
     38400	 26.042      1.302   //目前us级延时能够支持的最大波特率
     56000	 19.857      0.993
     57600	 17.361      0.868
     115200	 8.681       0.434
     128000	 7.8125      0.391
     230400	 4.34        0.217
*/


__INLINE_STATIC_ void SUART_Init(FW_UART_Type *dev)
{
    u16 tx_pin = dev->TX_Pin;
    u16 rx_pin = dev->RX_Pin;
    
    FW_GPIO_Init(tx_pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_High);
    FW_GPIO_Init(rx_pin, FW_GPIO_Mode_IPN, FW_GPIO_Speed_High);
    
    dev->SDTime = 1000000 / dev->Baudrate;
    
    FW_GPIO_SET(tx_pin);
}

__INLINE_STATIC_ void SUART_TX_Byte(FW_UART_Type *dev, u8 value)
{
    u16 tx_pin = dev->TX_Pin;
    u16 time = dev->SDTime;
    u8 bits = (dev->Data_Bits == 0) ? 8 : dev->Data_Bits;
    u8 parity = 0;
    u8 i;
    
    /* 起始位 */
    FW_GPIO_CLR(tx_pin);
    FW_Delay_Us(time);
    
    /* 数据 */
    for(i = 0; i < bits; i++)
    {
        if(value & 0x01)
            FW_GPIO_SET(tx_pin);
        else
            FW_GPIO_CLR(tx_pin);
        FW_Delay_Us(time);
        value >>= 1;
        parity += 1;
    }
    
    /* 校验位 */
    if(dev->Parity != FW_UART_Parity_None)
    {
        if(dev->Parity == FW_UART_Parity_Odd)    //奇校验
        {
            if(parity % 2)
                FW_GPIO_CLR(tx_pin);
            else
                FW_GPIO_SET(tx_pin);
        }
        else                                     //偶校验
        {
            if(parity % 2) 
                FW_GPIO_SET(tx_pin);
            else
                FW_GPIO_CLR(tx_pin);
        }
        FW_Delay_Us(time);
    }
    
    /* 停止位 */
    switch(dev->Stop_Bits)
    {
        case FW_UART_StopBits_0_5:
            time >>= 1;
            break;
        
        case FW_UART_StopBits_1:
            break;
        
        case FW_UART_StopBits_1_5:
            time = time + (time >> 1);
            break;
        
        case FW_UART_StopBits_2:
            time <<= 1;
            break;
        
        default:
            break;
    }
    FW_GPIO_SET(tx_pin);
    FW_Delay_Us(time);
}

/* 模拟读是阻塞等待 */
__INLINE_STATIC_ u8   SUART_RX_Byte(FW_UART_Type *dev)
{
    u16 rx_pin = dev->RX_Pin;
    u16 time = dev->SDTime;
    u8 bits = (dev->Data_Bits == 0) ? 8 : dev->Data_Bits;
    u8 parity = 0;
    u8 value = 0;
    u8 i;
    
    /* 起始位 */
    FW_Delay_Us(time);
    
    /* 数据 */
    for(i = 0; i < bits; i++)
    {
        FW_Delay_Us(time >> 1);
        if(FW_GPIO_Read(rx_pin) == LEVEL_H)  value |= 1 << i;
        FW_Delay_Us(time >> 1);
        parity += 1;
    }
    
    /* 校验位 */
    if(dev->Parity != FW_UART_Parity_None)
    {
        FW_Delay_Us(time);
    }
    
    /* 停止位，协调收发同步，接收不处理 */
    
    return value;
}

__INLINE_STATIC_ u8   SUART_Wait_RC(FW_UART_Type *dev)
{
    u16 rx_pin = dev->RX_Pin;
    if(FW_GPIO_Read(rx_pin) == LEVEL_H)  return False;
    return True;
}



/* IO模拟UART驱动 */
__CONST_STATIC_ FW_UART_Driver_Type SUART_Driver = 
{
    .Init = SUART_Init,
    
    .TX_Byte = SUART_TX_Byte,
    .RX_Byte = SUART_RX_Byte,
    
    .Wait_RC = SUART_Wait_RC,
};
FW_DRIVER_REGIST("io->uart", &SUART_Driver, SUART);



/* UART设备参数复制，duart：目标设备；suart：源设备 */
#define UART_DP_COPY(duart, suart)\
    (duart)->TX_Pin = (suart)->TX_Pin;\
    (duart)->RX_Pin = (suart)->RX_Pin;\
    (duart)->Baudrate = (suart)->Baudrate;\
    (duart)->Data_Bits = (suart)->Data_Bits;\
    (duart)->Stop_Bits = (suart)->Stop_Bits;\
    (duart)->Parity = (suart)->Parity;\
    (duart)->SDTime = (suart)->SDTime;



void FW_SUART_Init(FW_SUART_Type *dev)
{
    FW_UART_Type puart;
    
    UART_DP_COPY(&puart, dev);
    SUART_Init(&puart);
    UART_DP_COPY(dev, &puart);
}

u32  FW_SUART_Write(FW_SUART_Type *dev, const u8 *pdata, u32 num)
{
    FW_UART_Type puart;
    u32 i;
    
    UART_DP_COPY(&puart, dev);
    
    for(i = 0; i < num; i++)
    {
        SUART_TX_Byte(&puart, *pdata++);
    }
    
    return num;
}

u32  FW_SUART_Read(FW_SUART_Type *dev, u8 *pdata, u32 num)
{
    FW_UART_Type puart;
    u32 i;
    
    UART_DP_COPY(&puart, dev);
    
    for(i = 0; i < num; i++)
    {
        SUART_Wait_RC(&puart);
        *pdata++ = SUART_RX_Byte(&puart);
    }
    
    return num;
}


#if defined(SUART_PRINT_EN) && (SUART_PRINT_EN)

#define PDEV_NAME       "puart"

//static FW_SUART_Type PUART;

//static void PUART_Config(void *pdata)
//{
//    FW_SUART_Type *dev = pdata;
//    
//    dev->Baudrate = 9600;
//    dev->
//}


static FW_SUART_Type *Printer;

#else

static FW_SUART_Type *Printer;

#endif  /* #if defined(SUART_PRINT_EN) && (SUART_PRINT_EN) */


static char Print_Buffer[64];


void FW_SUART_PrintInit(FW_SUART_Type *dev)
{
    Printer = dev;
}

int FW_SUART_Printf(const char *format, ...)
{
    va_list args;
    u32 num;
    
    __va_start(args, format);
    num = vsnprintf(Print_Buffer, sizeof(Print_Buffer), format, args);
    __va_end(args);
    
    FW_SUART_Write(Printer, (u8 *)Print_Buffer, num);
    
    return 0;
}



/* Test */
#include "project_debug.h"
#if MODULE_TEST && SUART_TEST
#include "fw_system.h"
#include <stdio.h>


/* 定义设备ID */
//#define LOG_DEV_ID  FW_DEVICE_ID(FW_Device_UART, 0x10, 0, NULL)
#define LOG_DEV_ID  "log"

/* 定义一个UART设备(对象) */
static FW_UART_Type Log_UART;

/* 定义Log_UART的初始配置函数 */
static void Log_UART_Config(void *dev)
{
    FW_UART_Driver_Type *drv = FW_Driver_Find(SUART_DRV_ID);
    
    Log_UART.Baudrate = 19200;
    Log_UART.TX_Pin = PA9;
    Log_UART.RX_Pin = PA10;
    
    FW_Device_SetDriver(&Log_UART, drv);
    FW_Device_SetDID(&Log_UART, SUART_DRV_ID);
}

/* 注册设备，设备自动关联相应的驱动 */
FW_DEVICE_STATIC_REGIST(LOG_DEV_ID, &Log_UART, Log_UART_Config, UART_Log);

void Test(void)
{
    FW_UART_Type *fd;
    u32 cnt = 0;
    u8 len;
    u8 msg[10];
    
    FW_System_Init();
    FW_Delay_Init();
    
    fd = FW_Device_Find(LOG_DEV_ID);
    if(fd == NULL)  while(1);
    
    FW_UART_Init(fd);
//    FW_UART_MapPrintf(fd);
    FW_UART_PrintInit(fd);
    
    printf("tx ok\r\n");
    
    while(1)
    {
        #if 0
        len = FW_UART_Read(fd, msg, 1);
        if(len)
        {
            printf("F2M uart printf test  %d %d\r\n", msg[0], cnt++);
        }
        #else
        len = FW_UART_Read(fd, msg, sizeof(msg));
        if(len)
        {
            printf("%s\r\n", msg);
//            printf("F2M uart printf test %d\r\n", cnt++);
        }
        #endif
    }
}

#endif



#if 0
#include "project_debug.h"
#if MODULE_TEST && SUART_TEST
#include "fw_system.h"



/*  */
#define COM_Sim0  FW_DEVICE_ID(FW_DT_UART, 0, 0x7A, NULL)

#define SUART0_DRV_ID  FW_DRIVER_ID(FW_DT_UART, 3)

/* 模拟UART驱动注册 */
FW_HP_REGISTER(SUART0_DRV_ID, &SUART_Driver, SUART0);
FW_DRIVER_REGISTER(SUART0_DRV_ID, &SUART_Driver, SUART0);


/* 定义设备的ID */
#define log_uart_id  FW_DEVICE_ID(FW_DT_UART, 0, 0x7A, NULL)

/* 定义一个UART对象 */
static FW_UART_Type Log_UART;

/* 定义Log_UART的初始配置函数 */
static void Log_UART_Config(void *dev)
{
    Log_UART.Baudrate = 9600;
    Log_UART.TX_Pin = PA9;
}

/* 注册设备，设备自动关联相应的驱动 */
FW_DEVICE_REGISTER(log_uart_id, &Log_UART, Log_UART_Config, UART_Log);


void Test(void)
{
    FW_UART_Type *fd;
    
    FW_System_Init();
    FW_Delay_Init();
    
    fd = FW_Device_Find(log_uart_id);
    if(fd == NULL)
    {
        while(1);
    }
    
    FW_UART_Init(fd);
    
}

#endif
#endif
