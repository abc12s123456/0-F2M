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
#include "a76xx.h"

#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"

#include "mqtt.h"


#define SEND_TIMEOUT         1000      //发送超时时间，单位：ms
#define SEND_WAIT_TIME       10        //发送等待时间，单位：ms
#define SEND_CNT_MAX         50        //发送次数
#define RESEND_CNT_MAX       2         //重发次数


#define A76_LOCK             FW_Lock
#define A76_UNLOCK           FW_Unlock


__INLINE_STATIC_ void A76_Clear_RBuff(A76_Type *dev)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    FW_UART_ClearFIFO(uart, TOR_RX);
}

__INLINE_STATIC_ void A76_Write_Cmd(A76_Type *dev, const char *cmd)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    FW_UART_Write(uart, 0, (u8 *)cmd, strlen(cmd));
}

__INLINE_STATIC_ void A76_Write_Msg(A76_Type *dev, const u8 *pdata, u32 num)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    FW_UART_Write(uart, 0, pdata, num);
}

__INLINE_STATIC_ u32  A76_Read_First(A76_Type *dev, const char *s, u8 *pdata, u32 num)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    return FW_UART_ReadFirst(uart, s, pdata, num);
}

__INLINE_STATIC_ u32  A76_Read_Inter(A76_Type *dev, const char *s1, const char *s2, u8 *pdata, u32 cntout)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    u8 len = FW_UART_ReadInter(uart, s1, s2, pdata, cntout);
    u8 s1_len = strlen(s1);
    u8 s2_len = strlen(s2);
    
    if(len)
    {
        len = len - s1_len - s2_len;
        memmove(pdata, pdata + s1_len, len);
        return len;
    }
    
    return 0;
}

__INLINE_STATIC_ Bool A76_String_IsExist(A76_Type *dev, const char *s)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    return FW_UART_StringIsExist(uart, s);
}


__INLINE_STATIC_ Bool AT_Send(A76_Type *dev, const char *cmd, const char *rcmp)
{
    u8 resend = 0;
    u8 cnt = 0;
    
    Send:
    A76_Clear_RBuff(dev);
    
    A76_Write_Cmd(dev, cmd);
    
    while(A76_String_IsExist(dev, rcmp) == False)
    {
        FW_Delay_Ms(SEND_WAIT_TIME);
        if(cnt++ > SEND_CNT_MAX)
        {
            cnt = 0;
            if(resend++ > RESEND_CNT_MAX)
            {
                return False;
            }
            goto Send;
        }
    }
    
    return True;
}



void A76_Init(A76_Type *dev)
{
    FW_UART_Type *uart = FW_Device_GetParent(dev);
    FW_UART_Init(uart);
}

void A76_Reset(A76_Type *dev)
{
    
}

static Bool A76_Switch_CmdMode(A76_Type *dev)
{
    return AT_Send(dev, "+++\r\n", "OK");
}

static Bool A76_Test(A76_Type *dev)
{
    return AT_Send(dev, "AT\r\n", "OK");
}

static Bool A76_Get_SIMCardStatus(A76_Type *dev)
{
    return AT_Send(dev, "AT+CPIN?\r\n", "OK");
}

static u8   A76_Get_RSSI(A76_Type *dev)
{

}

static u8   A76_Get_Server(A76_Type *dev)
{

}

static u8   A76_Get_PSService(A76_Type *dev)
{

}

static u8   A76_Get_UEInfo(A76_Type *dev)
{

}





void A76_MQTT_Init(MQTT_Type *mqtt)
{
    A76_Type *a76 = FW_Device_GetParent(mqtt);
    A76_Init(a76);
}

Bool A76_MQTT_Connect(MQTT_Type *mqtt, const char *link, const char *server)
{
    
}



#include "fw_debug.h"
#if MODULE_TEST && UART_DEVICE_TEST && A76XX_TEST


#define A76_UART_DEV_NAME            "uart1"
static void A76_UART_Pre_Init(void *pdata)
{
    FW_UART_Type *dev = FW_Device_Find(A76_UART_DEV_NAME);
    dev->TX_Pin = PA8;
    dev->RX_Pin = PA9;
//    dev->TX_Pin = PA5;
//    dev->RX_Pin = PA6;
    dev->Baudrate = 115200;
    dev->RX_RB_Size = 256;
    dev->Config.RX_Mode = TRM_INT;
}
FW_BOARD_INIT(A76_UART_Pre_Init, NULL);


#define A76_DEV_NAME         "a76xx"
static A76_Type A76xx;
static void A76_Config(void *dev)
{
    FW_UART_Type *uart = FW_Device_Find(A76_UART_DEV_NAME);
    FW_Device_SetParent(dev, uart);
    FW_Device_SetDriver(dev, FW_Driver_Find("uart->dev"));
}
FW_DEVICE_STATIC_REGIST(A76_DEV_NAME, &A76xx, A76_Config, A76xx);



void Test(void)
{
//    while(1);
    u32 start;
    u32 duration;
    
    A76_Type *dev = FW_Device_Find(A76_DEV_NAME);
    Bool flag;
    
    A76_Init(dev);
    
//    A76_Switch_CmdMode(dev);
    
    while(1)
    {
        flag = A76_Test(dev);
        if(flag)
        {
            flag = flag;
        }
//        
//        flag = A76_Get_SIMCardStatus(dev);
//        if(flag)
//        {
//            flag = flag;
//        }
        
        start = FW_Delay_GetMsStart();
        FW_Delay_Ms(10);
        duration = FW_Delay_GetMsDuration(start);
        if(duration)  duration = duration;
    }
}


#endif

