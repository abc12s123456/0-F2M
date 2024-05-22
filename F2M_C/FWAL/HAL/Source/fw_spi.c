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
 
#include "fw_spi.h"
#include "fw_system.h"
#include "fw_gpio.h"
#include "fw_debug.h"
#include "fw_print.h"

#include "libc.h"


static void FW_SPI_CSIOSet(FW_SPI_Type *dev, u8 state);

static void FW_SPI_IH_RX(void *dev);
static void FW_SPI_IH_RC(void *dev);
static void FW_SPI_IH_TX(void *dev);
static void FW_SPI_IH_TC(void *dev);

static u32  FW_SPI_WritePOL(void *dev, u32 offset, const void *pdata, u32 num);
static u32  FW_SPI_WriteINT(void *dev, u32 offset, const void *pdata, u32 num);
static u32  FW_SPI_WriteDMA(void *dev, u32 offset, const void *pdata, u32 num);

static u32  FW_SPI_ReadPOL(void *dev, u32 offset, void *pdata, u32 num);
static u32  FW_SPI_ReadINT(void *dev, u32 offset, void *pdata, u32 num);
static u32  FW_SPI_ReadDMA(void *dev, u32 offset, void *pdata, u32 num);


void FW_SPI_SetPort(FW_SPI_Type *dev, void *spi)
{
    if(dev->SPIx == NULL)  dev->SPIx = spi;
}

void *FW_SPI_GetPort(FW_SPI_Type *dev)
{
    return dev->SPIx;
}

u8   FW_SPI_GetTRM(FW_SPI_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_Mode : dev->Config.RX_Mode;
}

void *FW_SPI_GetDMABBase(FW_SPI_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_DMA->Buffer : dev->Config.RX_DMA->Buffer;
}

u32  FW_SPI_GetDMABSize(FW_SPI_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_DMA->Size : dev->Config.RX_DMA->Size;
}

void FW_SPI_DeInit(FW_SPI_Type *dev)
{

}

void FW_SPI_Init(FW_SPI_Type *dev)
{
    FW_SPI_Driver_Type *drv;
    SGTR_Type sgtr;
    
    drv = FW_Device_GetDriver(dev);
    if(drv == NULL)
    {
        LOG_D("%s driver is invalid\r\n", dev->Device.Name);
        return;
    }
    
    SGTR_DeInit(&sgtr);
    
    memcpy(&sgtr.Config, &dev->Config, sizeof(SGC_Type));
    
    sgtr.IH_RC = FW_SPI_IH_RC;
    sgtr.IH_RX = FW_SPI_IH_RX;
    sgtr.IH_TC = FW_SPI_IH_TC;
    sgtr.IH_TX = FW_SPI_IH_TX;
    
    sgtr.Write_POL = FW_SPI_WritePOL;
    sgtr.Write_INT = FW_SPI_WriteINT;
    sgtr.Write_DMA = FW_SPI_WriteDMA;
    
    sgtr.Read_POL = FW_SPI_ReadPOL;
    sgtr.Read_INT = FW_SPI_ReadINT;
    sgtr.Read_DMA = FW_SPI_ReadDMA;
    
    SGTR_Config(&sgtr);
    
    memcpy(&dev->Config, &sgtr.Config, sizeof(SGC_Type));
    
    /* CS_CTL若无底层实现，则使用IO软件操作 */
    if(drv->CS_Set == NULL)
    {
        if(dev->CS_VL == LEVEL_L)
        {
            FW_GPIO_Init(dev->CS_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
        }
        else
        {
            FW_GPIO_Init(dev->CS_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
        }
        dev->CS_Set = FW_SPI_CSIOSet;
    }
    else
    {
        dev->CS_Set = drv->CS_Set;
    }

    FW_ASSERT(drv->Init);
    drv->Init(dev);
}


void FW_SPI_CTL(FW_SPI_Type *dev, u8 state)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_ASSERT(drv->CTL);
    drv->CTL(dev, state);
}

void FW_SPI_TXCTL(FW_SPI_Type *dev, u8 state)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->TX_CTL(dev, state);
}

void FW_SPI_RXCTL(FW_SPI_Type *dev, u8 state)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->RX_CTL(dev, state);
}

u32  FW_SPI_Write(FW_SPI_Type *dev, u32 offset, const u8 *pdata, u32 num)
{
    return dev->Config.Write(dev, offset, pdata, num);
}

u32  FW_SPI_Read(FW_SPI_Type *dev, u32 offset, u8 *pdata, u32 num)
{
    return dev->Config.Read(dev, offset, pdata, num);
}

void FW_SPI_CSSet(FW_SPI_Type *dev, u8 state)
{
    dev->CS_Set(dev, state);
}


/*  */
static void FW_SPI_CSIOSet(FW_SPI_Type *dev, u8 state)
{
    if(state)
        FW_GPIO_Write(dev->CS_Pin, dev->CS_VL);
    else
        FW_GPIO_Write(dev->CS_Pin, !dev->CS_VL);
}


/* 中断服务处理 */
static void FW_SPI_IH_RX(void *dev)
{

}

static void FW_SPI_IH_RC(void *dev)
{

}

static void FW_SPI_IH_TX(void *dev)
{

}

static void FW_SPI_IH_TC(void *dev)
{

}


__INLINE_STATIC_ u8 FW_SPI_WRByte(FW_SPI_Type *dev, u8 value)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    u16 timeout;
    
    if(drv->Wait_TC)
    {
        timeout = 0;
        while(drv->Wait_TC(dev) != True)
        {
            if(timeout++ > 10000)  break;
        }
    }
    drv->TX_Byte(dev, value);
    
    if(drv->Wait_RC)
    {
        timeout = 0;
        while(drv->Wait_RC(dev) != True)
        {
            if(timeout++ > 10000)  break;
        }
    }
    
    return drv->RX_Byte(dev);
}

static u32  FW_SPI_WritePOL(void *dev, u32 offset, const void *pdata, u32 num)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    u8 *p = (u8 *)pdata;
    u32 i;
    
    if(drv->Write)  return drv->Write(dev, pdata, num);
    
    for(i = 0; i < num; i++)
    {
        FW_SPI_WRByte(dev, *p++);
    }
    
    return num;
}

static u32  FW_SPI_WriteINT(void *dev, u32 offset, const void *pdata, u32 num)
{
    return 0;
}

static u32  FW_SPI_WriteDMA(void *dev, u32 offset, const void *pdata, u32 num)
{
    return 0;
}

static u32  FW_SPI_ReadPOL(void *dev, u32 offset, void *pdata, u32 num)
{
    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    u8 *p = (u8 *)pdata;
    u32 i;
    
    if(drv->Read)  return drv->Read(dev, pdata, num);
    
    for(i = 0; i < num; i++)
    {
        *p++ = FW_SPI_WRByte(dev, 0xFF);
    }
    
    return num;
}

static u32  FW_SPI_ReadINT(void *dev, u32 offset, void *pdata, u32 num)
{
    return 0;
}

static u32  FW_SPI_ReadDMA(void *dev, u32 offset, void *pdata, u32 num)
{
    return 0;
}




///* SPI通用设备驱动 */
//__INLINE_STATIC_ void SPI_GPD_Init(FW_SPI_Type *dev)
//{
//    FW_SPI_Init(dev);
//}

//__INLINE_STATIC_ u32  SPI_GPD_Write(FW_SPI_Type *dev, u32 offset, const u8 *pdata, u32 num)
//{
//    return FW_SPI_Write(dev, offset, pdata, num);
//}

//__INLINE_STATIC_ u32 SPI_GPD_Read(FW_SPI_Type *dev, u32 offset, u8 *pdata, u32 num)
//{
//    return FW_SPI_Read(dev, offset, pdata, num);
//}

//__INLINE_STATIC_ void SPI_GPD_CSCTL(FW_SPI_Type *dev, u8 state)
//{
//    dev->CS_CTL(dev, state);
//}




///* SPI I2C GP(General Periphal) Device Driver */
//__CONST_STATIC_ SPI_GP_Driver_Type SPI_GPD_Driver =
//{
//    .Init   = SPI_GPD_Init,
//    .Write  = SPI_GPD_Write,
//    .Read   = SPI_GPD_Read,
//    .CS_CTL = SPI_GPD_CSCTL,
//};
//FW_DRIVER_REGIST("spi->dev", &SPI_GPD_Driver, SPI_GPD);

