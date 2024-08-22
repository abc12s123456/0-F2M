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


static void FW_CAN_IH_RX(void *dev);
static void FW_CAN_IH_TX(void *dev);

static u32  FW_CAN_WritePOL(void *dev, u32 ptr, const void *pdata, u32 num);
static u32  FW_CAN_WriteINT(void *dev, u32 ptr, const void *pdata, u32 num);
static u32  FW_CAN_WriteDMA(void *dev, u32 ptr, const void *pdata, u32 num);

static u32  FW_CAN_ReadPOL(void *dev, u32 ptr, void *pdata, u32 num);
static u32  FW_CAN_ReadINT(void *dev, u32 ptr, void *pdata, u32 num);
static u32  FW_CAN_ReadDMA(void *dev, u32 ptr, void *pdata, u32 num);



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
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    if(drv == NULL)
    {
        LOG_D("%s driver is invalid\r\n", dev->Device.Name);
        return;
    }
    
    FW_ASSERT(drv->DeInit);
    drv->DeInit(dev);
}

void FW_CAN_Init(FW_CAN_Type *dev)
{
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    SGTR_Type sgtr;
    
    if(drv == NULL)
    {
        LOG_D("%s driver is invalid\r\n", dev->Device.Name);
        return;
    }
    
    SGTR_DeInit(&sgtr);
    
    memcpy(&sgtr.Config, &dev->Config, sizeof(SGC_Type));
    
//    sgtr.IH_RC = FW_CAN_IH_RC;
    sgtr.IH_RX = FW_CAN_IH_RX;
//    sgtr.IH_TC = FW_CAN_IH_TC;
    sgtr.IH_TX = FW_CAN_IH_TX;
    
    sgtr.Write_POL = FW_CAN_WritePOL;
    sgtr.Write_INT = FW_CAN_WriteINT;
    sgtr.Write_DMA = FW_CAN_WriteDMA;
    
    sgtr.Read_POL = FW_CAN_ReadPOL;
    sgtr.Read_INT = FW_CAN_ReadINT;
    sgtr.Read_DMA = FW_CAN_ReadDMA;
    
    SGTR_Config(&sgtr);
    
    memcpy(&dev->Config, &sgtr.Config, sizeof(SGC_Type));
    
    FW_ASSERT(drv->Init);
    drv->Init(dev);
}

void FW_CAN_CTL(FW_CAN_Type *dev, u8 state)
{
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->CTL);
    drv->CTL(dev, state);
}

void FW_CAN_TXCTL(FW_CAN_Type *dev, u8 state)
{
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->TX_CTL);
    drv->TX_CTL(dev, state);
}

void FW_CAN_RXCTL(FW_CAN_Type *dev, u8 state)
{
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->RX_CTL);
    drv->RX_CTL(dev, state);
}

u32  FW_CAN_Write(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame, const u8 *pdata, u32 num)
{
    return dev->Config.Write(dev, (u32)frame, pdata, num);
}

u32  FW_CAN_Read(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame, u8 *pdata, u32 num)
{
    return dev->Config.Read(dev, (u32)frame, pdata, num);
}

void FW_CAN_RX_ISR(FW_CAN_Type *dev)
{
    if(dev->Config.IH_RX)  dev->Config.IH_RX(dev);
}

void FW_CAN_TX_ISR(FW_CAN_Type *dev)
{
    if(dev->Config.IH_TX)  dev->Config.IH_TX(dev);
}




/* ÖÐ¶Ï·þÎñ´¦Àí */
static void FW_CAN_IH_RX(void *dev)
{
    FW_CAN_Type *can = dev;
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(can);
    RB_Type *fifo = can->Config.RX_FIFO;
    FW_CAN_Frame_Type frame;
    
    FW_Lock();
    if(fifo->Size)
    {
        drv->Frame_Read(dev, &frame);
        RB_Write(fifo, (u8 *)&frame, sizeof(frame));
    }
    FW_Unlock();
}

static void FW_CAN_IH_TX(void *dev)
{
    
}

__INLINE_STATIC_ u8 FW_CAN_WaitNull(FW_CAN_Type *dev){return True;}

static u32  FW_CAN_WritePOL(void *dev, u32 ptr, const void *pdata, u32 num)
{
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_CAN_Frame_Type *frame = (FW_CAN_Frame_Type *)ptr;
    u32 i;
    u32 len;
    
    u8 *p = (u8 *)pdata;
    u8 (*pf)(FW_CAN_Type *) = FW_CAN_WaitNull;
    
    if(drv->Wait_TC)  pf = drv->Wait_TC;
    
    len = num - (num % 8);
    
    for(i = 0; i < len; i += 8)
    {
        memcpy(frame->Data, p, 8);
        drv->Frame_Write(dev, frame);
        while(pf(dev) != True);
        p += 8;
    }
    
    if(len < num)
    {
        memcpy(frame->Data, pdata, num - len);
        drv->Frame_Write(dev, frame);
        while(pf(dev) != True);
    }
    
    return num;
}

static u32  FW_CAN_WriteINT(void *dev, u32 ptr, const void *pdata, u32 num)
{
    return num;
}

static u32  FW_CAN_WriteDMA(void *dev, u32 ptr, const void *pdata, u32 num)
{
    return num;
}

static u32  FW_CAN_ReadPOL(void *dev, u32 ptr, void *pdata, u32 num)
{    
    FW_CAN_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_CAN_Frame_Type *frame = (FW_CAN_Frame_Type *)ptr;
    FW_CAN_Frame_Type rmsg;
    u32 i;
    u32 len;
    
    u8 *p = (u8 *)pdata;
    u8 (*pf)(FW_CAN_Type *) = FW_CAN_WaitNull;
    
    if(drv->Wait_RC)  pf = drv->Wait_RC;

    len = 0;
    
    while(1)
    {
        while(pf(dev) != True);
        drv->Frame_Read(dev, &rmsg);
        if(rmsg.Type == frame->Type && rmsg.Format == frame->Format && rmsg.Length)
        {
            memcpy(p, rmsg.Data, rmsg.Length);
            len += rmsg.Length;
            if(len >= num)  break;
        }
    }
    
    return num;
}

static u32  FW_CAN_ReadINT(void *dev, u32 ptr, void *pdata, u32 num)
{
    FW_CAN_Type *can = dev;
    return RB_Read(can->Config.RX_FIFO, pdata, num * sizeof(FW_CAN_Frame_Type));
}

static u32  FW_CAN_ReadDMA(void *dev, u32 ptr, void *pdata, u32 num)
{
    return num;
}

