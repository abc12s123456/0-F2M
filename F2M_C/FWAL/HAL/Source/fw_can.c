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
#include "fw_can.h"
#include "fw_system.h"
#include "fw_gpio.h"
#include "fw_debug.h"
#include "fw_print.h"

#include "libc.h"



void FW_CAN_SetPort(FW_CAN_Type *dev, void *can)
{
    if(dev->CANx == NULL)  dev->CANx = can;
}

void *FW_CAN_GetPort(FW_CAN_Type *dev)
{
    return dev->CANx;
}

u8   FW_CAN_GetTRM(FW_CAN_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_Mode : dev->Config.RX_Mode;
}

void FW_CAN_SetTRM(FW_CAN_Type *dev, u8 tr, u8 mode)
{
    if(tr == TOR_TX)
        dev->Config.TX_Mode = mode;
    else
        dev->Config.RX_Mode = mode;
}

void FW_CAN_DeInit(FW_CAN_Type *dev)
{

}

void FW_CAN_Init(FW_CAN_Type *dev)
{

}

void FW_CAN_CTL(FW_CAN_Type *dev, u8 state)
{

}

void FW_CAN_TXCTL(FW_CAN_Type *dev, u8 state)
{

}

void FW_CAN_RXCTL(FW_CAN_Type *dev, u8 state)
{

}

u32  FW_CAN_Write(FW_CAN_Type *dev, u32 offset, const u8 *pdata, u32 num)
{

}

u32  FW_CAN_Read(FW_CAN_Type *dev, u32 offset, u8 *pdata, u32 num)
{

}

void FW_CAN_RX_ISR(FW_CAN_Type *dev)
{

}

void FW_CAN_TX_ISR(FW_CAN_Type *dev)
{

}










