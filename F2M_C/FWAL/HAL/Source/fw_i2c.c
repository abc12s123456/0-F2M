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
 
#include "fw_i2c.h"
#include "fw_system.h"
#include "fw_gpio.h"
#include "fw_debug.h"
#include "fw_print.h"

#include "libc.h"


static void FW_I2C_IH_RX(void *dev);
static void FW_I2C_IH_RC(void *dev);
static void FW_I2C_IH_TX(void *dev);
static void FW_I2C_IH_TC(void *dev);

static u32  FW_I2C_WritePOL(void *dev, u32 addr, const void *pdata, u32 num);
static u32  FW_I2C_WriteINT(void *dev, u32 addr, const void *pdata, u32 num);
static u32  FW_I2C_WriteDMA(void *dev, u32 addr, const void *pdata, u32 num);

static u32  FW_I2C_ReadPOL(void *dev, u32 addr, void *pdata, u32 num);
static u32  FW_I2C_ReadINT(void *dev, u32 addr, void *pdata, u32 num);
static u32  FW_I2C_ReadDMA(void *dev, u32 addr, void *pdata, u32 num);


void FW_I2C_SetPort(FW_I2C_Type *dev, void *i2c)
{
    if(dev->I2Cx == NULL)  dev->I2Cx = i2c;
}

void *FW_I2C_GetPort(FW_I2C_Type *dev)
{
    return dev->I2Cx;
}

u8   FW_I2C_GetTRM(FW_I2C_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_Mode : dev->Config.RX_Mode;
}

void *FW_I2C_GetDMABBase(FW_I2C_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_DMA->Buffer : dev->Config.RX_DMA->Buffer;
}

u32  FW_I2C_GetDMABSize(FW_I2C_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_DMA->Size : dev->Config.RX_DMA->Size;
}


void FW_I2C_DeInit(FW_I2C_Type *dev)
{

}

void FW_I2C_Init(FW_I2C_Type *dev)
{
    FW_I2C_Driver_Type *drv;
    SGTR_Type sgtr;
    
    drv = FW_Device_GetDriver(dev);
    if(drv == NULL)
    {
        LOG_D("%s driver is invalid\r\n", dev->Device.Name);
        return;
    }
    
    SGTR_DeInit(&sgtr);
    
    memcpy(&sgtr.Config, &dev->Config, sizeof(SGC_Type));
    
    sgtr.IH_RC = FW_I2C_IH_RC;
    sgtr.IH_RX = FW_I2C_IH_RX;
    sgtr.IH_TC = FW_I2C_IH_TC;
    sgtr.IH_TX = FW_I2C_IH_TX;
    
    sgtr.Write_POL = FW_I2C_WritePOL;
    sgtr.Write_INT = FW_I2C_WriteINT;
    sgtr.Write_DMA = FW_I2C_WriteDMA;
    
    sgtr.Read_POL = FW_I2C_ReadPOL;
    sgtr.Read_INT = FW_I2C_ReadINT;
    sgtr.Read_DMA = FW_I2C_ReadDMA;
    
    SGTR_Config(&sgtr);
    
    memcpy(&dev->Config, &sgtr.Config, sizeof(SGC_Type));
    
    FW_ASSERT(drv->Init);
    drv->Init(dev);
}

void FW_I2C_CTL(FW_I2C_Type *dev, u8 state)
{
    FW_I2C_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->CTL);
    drv->CTL(dev, state);
}


u32  FW_I2C_Write(FW_I2C_Type *dev, u32 addr, const u8 *pdata, u32 num)
{
    return dev->Config.Write(dev, addr, pdata, num);
}

u32  FW_I2C_Read(FW_I2C_Type *dev, u32 addr, u8 *pdata, u32 num)
{
    return dev->Config.Read(dev, addr, pdata, num);
}


void FW_I2C_SetDelay(FW_I2C_Type *dev, u8 d0, u8 d1, u8 d2, u8 d3)
{
    dev->D0 = d0;
    dev->D1 = d1;
    dev->D2 = d2;
    dev->D3 = d3;
}


static void FW_I2C_IH_RX(void *dev)
{

}

static void FW_I2C_IH_RC(void *dev)
{

}

static void FW_I2C_IH_TX(void *dev)
{
    
}

static void FW_I2C_IH_TC(void *dev)
{
    
}


static u32  FW_I2C_WritePOL(void *dev, u32 addr, const void *pdata, u32 num)
{
    FW_I2C_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->Write);
    return drv->Write(dev, addr, pdata, num);
}

static u32  FW_I2C_WriteINT(void *dev, u32 addr, const void *pdata, u32 num)
{
    return 0;
}

static u32  FW_I2C_WriteDMA(void *dev, u32 addr, const void *pdata, u32 num)
{
    return 0;
}


static u32  FW_I2C_ReadPOL(void *dev, u32 addr, void *pdata, u32 num)
{
    FW_I2C_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->Read);
    return drv->Read(dev, addr, pdata, num);
}

static u32  FW_I2C_ReadINT(void *dev, u32 addr, void *pdata, u32 num)
{
    return 0;
}

static u32  FW_I2C_ReadDMA(void *dev, u32 addr, void *pdata, u32 num)
{
    return 0;
}

