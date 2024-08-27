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
 
#include "fw_flash.h"

#include "fw_debug.h"
#include "fw_print.h"

#include "fw_spi.h"
#include "fw_gpio.h"




/* 存储、扇区、页大小 */
#define SNOR_CHIP_BASE                      0
#define SNOR_CHIP_SIZE                      (8 * 1024 * 1024)   //芯片存储大小
#define SNOR_SECTOR_SIZE                    4096                //扇区大小(最小擦除单位)
#define SNOR_PAGE_SIZE                      256                 //页大小(最小写入单位)

/* 操作指令 */
#define SNOR_CMD_WREN                       0x06                //写使能
#define SNOR_CMD_WRDI                       0x04                //写禁止

#define SNOR_CMD_RDSR0                      0x05                //读状态寄存器[7:0]
#define SNOR_CMD_RDSR1                      0x35                //读状态寄存器[15:8]
#define SNOR_CMD_RDSR2                      0x15                //读状态寄存器[23:16]

#define SNOR_CMD_WRSR0                      0x01                //写状态寄存器[7:0]
#define SNOR_CMD_WRSR1                      0x31                //写状态寄存器[15:8]
#define SNOR_CMD_WRSR2                      0x11                //写状态寄存器[23:16]

#define SNOR_CMD_EWSR                       0x50                //使能写状态寄存器

#define SNOR_CMD_READ                       0x03                //读数据(1Byte)
#define SNOR_CMD_FAST_READ                  0x0B                //高速读数据

#define SNOR_CMD_DUAL_OUTPUT_FAST_READ      0x3B                //双线输出时高速读数据
#define SNOR_CMD_DUAL_IO_FAST_READ          0xBB

#define SNOR_CMD_QUAD_OUTPUT_FAST_READ      0x6B
#define SNOR_CMD_QUAD_IO_FAST_READ          0xEB                //
#define SNOR_CMD_QUAD_IO_WORD_FAST_READ     0xE7
#define SNOR_CMD_QUAD_PP                    0x32                //Quad Page Program(4B)

#define SNOR_CMD_SE                         0x20                //扇区擦除
#define SNOR_CMD_CE                         0xC7                //全片擦除

#define SNOR_CMD_ENTER_DP                   0xB9                //Deep Power-Down
#define SNOR_CMD_EXIT_DP                    0xAB                //退出Deep Power-Down

#define SNOR_CMD_PP                         0x02                //Page Program

#define SNOR_CMD_GET_UID                    0x4B                //获取Unique ID
#define SNOR_CMD_GET_CID                    0x9F                //获取芯片ID

#define SNOR_CMD_SET_ADDR                   0xAF                //设置地址(非IC自身地址)


/* SPI AUX */
__INLINE_STATIC_ void SGP_EN(SNOR_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_CSSet(spi, Enable);
}

__INLINE_STATIC_ void SGP_DI(SNOR_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_CSSet(spi, Disable);
}

__INLINE_STATIC_ void SGP_Write_Byte(SNOR_Type *dev, u8 value)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_Write(spi, 0, &value, 1);
}

__INLINE_STATIC_ u8   SGP_Read_Byte(SNOR_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 value;
    FW_SPI_Read(spi, 0, &value, 1);
    return value;
}

__INLINE_STATIC_ void SGP_WREN(SNOR_Type *dev)
{
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_WREN);
    SGP_DI(dev);
}

__INLINE_STATIC_ void SGP_WRDI(SNOR_Type *dev)
{
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_WRDI);
    SGP_DI(dev);
}

/* 读取RDSR0 */
__INLINE_STATIC_ u8   SGP_RDSR(SNOR_Type *dev)
{
    u8 sr;
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_RDSR0);
    sr = SGP_Read_Byte(dev);
    SGP_DI(dev);
    return sr;
}

__INLINE_STATIC_ ffse SGP_Busy_Wait(SNOR_Type *dev)
{
    u16 i = 0;
    while((SGP_RDSR(dev) & 0x01) == 0x01)
    {
        if(i++ > 10000)
        {
            return FW_Flash_State_Timeout;
        }
    }
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ void SGP_EWSR(SNOR_Type *dev)
{
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_EWSR);
    SGP_DI(dev);
}

__INLINE_STATIC_ void SGP_WRSR(SNOR_Type *dev)
{
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_WRSR0);
    SGP_Write_Byte(dev, 0x00);             //取消所有存储单元写保护
    SGP_DI(dev);
}


/**
 * @使用SPI驱动SNOR
 */
__INLINE_STATIC_ void SGP_Init(SNOR_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);

    if(dev->Hold_Pin)
    {
        if(dev->Hold_VL == LEVEL_H)
        {
            /* Hold Pin默认设置为高，不进行数据保持 */
            FW_GPIO_Init(dev->Hold_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            FW_GPIO_SET(dev->Hold_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->Hold_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            FW_GPIO_CLR(dev->Hold_Pin);
        }
    }
    
    if(dev->WP_Pin)
    {
        if(dev->WP_VL == LEVEL_H)
        {
            /* WP Pin默认设置为高，允许读写 */
            FW_GPIO_Init(dev->WP_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            FW_GPIO_SET(dev->WP_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->WP_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            FW_GPIO_CLR(dev->WP_Pin);
        }
    }
    
    FW_SPI_Init(spi);

    /* 进行一次空写读 */
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_GET_CID);
    SGP_Read_Byte(dev);
    SGP_DI(dev);
}

__INLINE_STATIC_ ffst SGP_Get_CID(SNOR_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    ffst space;
    
    space.Base = 0;
    space.Size = 0;
    
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_GET_CID);
    FW_SPI_Read(spi, 0, (u8 *)&space.Base, 3);
    SGP_DI(dev);
    
    return space;
}

__INLINE_STATIC_ ffse SGP_Erase_Chip(SNOR_Type *dev)
{
    ffse state;
    
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    SGP_WREN(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_CE);
    SGP_DI(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ ffse SGP_Erase_Sector(SNOR_Type *dev, u32 addr)
{
    ffse state;
    
    addr /= SNOR_SECTOR_SIZE;
    addr *= SNOR_SECTOR_SIZE;
    
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    SGP_WREN(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_SE);
    
    /* 为避免大小端问题，addr按字节发送 */
    SGP_Write_Byte(dev, (u8)(addr >> 16));
    SGP_Write_Byte(dev, (u8)(addr >> 8));
    SGP_Write_Byte(dev, (u8)addr);
    SGP_DI(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ ffse SGP_Write(SNOR_Type *dev, u32 addr, const void *pdata, u32 num)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    ffse state;
    
    if(num == 0)  return FW_Flash_State_OK;
    
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    /* 使能写寄存器 */
    SGP_EWSR(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    /* 写寄存器 */
    SGP_WRSR(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    /* 写使能 */
    SGP_WREN(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    /* 写数据 */
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_PP);
    
    SGP_Write_Byte(dev, (u8)(addr >> 16));
    SGP_Write_Byte(dev, (u8)(addr >> 8));
    SGP_Write_Byte(dev, (u8)addr);
    
    /* 写入与读取使用相同的序列，则不受大小端影响 */
    FW_SPI_Write(spi, 0, (u8 *)pdata, num);
    SGP_DI(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    /* 写禁止 */
    SGP_WRDI(dev);
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return state;
    }
    
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ u32  SGP_Read(SNOR_Type *dev, u32 addr, void *pdata, u32 num)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    
    ffse state;
    
    if(num == 0)  return 0;
    
    state = SGP_Busy_Wait(dev);
    if(state != FW_Flash_State_OK)  
    {
        return 0;
    }
    
    SGP_EN(dev);
    SGP_Write_Byte(dev, SNOR_CMD_READ);
    SGP_Write_Byte(dev, (u8)(addr >> 16));
    SGP_Write_Byte(dev, (u8)(addr >> 8));
    SGP_Write_Byte(dev, (u8)addr);
    num = FW_SPI_Read(spi, 0, (u8 *)pdata, num);
    SGP_DI(dev);
    
    return num;
}


/* SNOR For Flash Driver */
__INLINE_STATIC_ void SNOR_Init(FW_Flash_Type *dev)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    void *p, *q;
    
    p = FW_Device_GetDriver(snor);
    if(p == NULL)
    {
        LOG_D("nor flash %d driver is null\r\n", dev_id);
        return;
    }
    
    q = FW_Driver_Find("spi->dev");
    if(q && p == q)
    {
        /* SPI_GPD_Driver */
        snor->Init         = SGP_Init;
        snor->Get_CID      = SGP_Get_CID;
        snor->Erase_Chip   = SGP_Erase_Chip;
        snor->Erase_Sector = SGP_Erase_Sector;
        snor->Write        = SGP_Write;
        snor->Read         = SGP_Read;
    }
    else
    {
        /* Direct Driver */
        SNOR_Driver_Type *drv = FW_Device_GetDriver(dev);
        
        snor->Init         = drv->Init;
        snor->Get_CID      = drv->Get_CID;
        snor->Erase_Chip   = drv->Erase_Chip;
        snor->Erase_Sector = drv->Erase_Sector;
        snor->Write        = drv->Write;
        snor->Read         = drv->Read;
    }
    
    snor->Init(snor);
}

__INLINE_STATIC_ u32 SNOR_Get_SectorNum(FW_Flash_Type *dev)
{
    return SNOR_CHIP_SIZE / SNOR_SECTOR_SIZE;
}

__INLINE_STATIC_ u32 SNOR_Get_SectorBase(FW_Flash_Type *dev, u32 sector_index)
{
    return sector_index * SNOR_SECTOR_SIZE;
}

__INLINE_STATIC_ u32 SNOR_Get_SectorSize(FW_Flash_Type *dev, u32 sector_index)
{
    return SNOR_SECTOR_SIZE;
}

__INLINE_STATIC_ ffst SNOR_Get_Main(FW_Flash_Type *dev)
{
    ffst space;
    space.Base = SNOR_CHIP_BASE;
    space.Size = SNOR_CHIP_SIZE;
    return space;
}

__INLINE_STATIC_ ffst SNOR_Get_CID(FW_Flash_Type *dev)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    return snor->Get_CID(snor);
}

__INLINE_STATIC_ Bool SNOR_Get_PEState(FW_Flash_Type *dev)
{
    return False;
}

__INLINE_STATIC_ ffse SNOR_Erase_Chip(FW_Flash_Type *dev)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    return snor->Erase_Chip(snor);
}

__INLINE_STATIC_ ffse SNOR_Erase_Sector(FW_Flash_Type *dev, u32 addr)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    return snor->Erase_Sector(snor, addr);
}

/* 最小单元写入(256B) */
__INLINE_STATIC_ ffse SNOR_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    
    ffse state;
    u32 ntmp;
    u8 *p = (u8 *)pdata;
    
    ntmp = num / SNOR_PAGE_SIZE;       //待写入的页数量
    while(ntmp--)
    {
        state = snor->Write(snor, addr, p, SNOR_PAGE_SIZE);
        if(state != FW_Flash_State_OK)  return state;
        addr += SNOR_PAGE_SIZE;
        p += SNOR_PAGE_SIZE;
    }
    
    ntmp = num % SNOR_PAGE_SIZE;
    if(ntmp)                           //还有剩余不足一页的数据待写入
    {
        u8 buffer[SNOR_PAGE_SIZE];
        num = snor->Read(snor, addr, buffer, SNOR_PAGE_SIZE);
        if(num == 0)  return FW_Flash_State_DataErr;
        memcpy(buffer, p, ntmp);
        state = snor->Write(snor, addr, buffer, SNOR_PAGE_SIZE);
        if(state != FW_Flash_State_OK)  return state;
    }
    
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ u32 SNOR_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num)
{
    SNOR_Type *snor = FW_Device_GetParent(dev);
    return snor->Read(snor, addr, pdata, num);
}


/**
 * @ Flash Driver
 */
__CONST_STATIC_ FW_Flash_Driver_Type GD25Q64_Driver =
{
    .Init           = SNOR_Init,
    
    .Get_SectorNum  = SNOR_Get_SectorNum,
    .Get_SectorBase = SNOR_Get_SectorBase,
    .Get_SectorSize = SNOR_Get_SectorSize,
    
    .Get_Main       = SNOR_Get_Main,
    .Get_CID        = SNOR_Get_CID,
    
    .Get_PE         = SNOR_Get_PEState,
    
    .Erase_Chip     = SNOR_Erase_Chip,
    .Erase_Sector   = SNOR_Erase_Sector,
    
    .Write          = SNOR_Write,
    .Read           = SNOR_Read,
};
FW_DRIVER_REGIST("25q64->flash", &GD25Q64_Driver, GD25Q64);




#include "project_debug.h"
#if MODULE_TEST && SPI_DEVICE_TEST && GD25X_TEST
#include "fw_system.h"
#include "fw_delay.h"
#include "fw_usb.h"

#include "mm.h"

#include <string.h>


static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(&SSPI0, drv);
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);


#define SNOR_SPI_NAME        "spi0"
//#define SNOR_SPI_NAME        "sspi0"
static  SNOR_Type GD25Q64;

static void GD25Q64_Config(void *dev)
{
    SNOR_Type *snor = dev;
    
    FW_Device_SetParent(snor, FW_Device_Find(SNOR_SPI_NAME));
    FW_Device_SetDriver(snor, FW_Driver_Find("spi->dev"));
    
    snor->WP_Pin = PB2;
}
FW_DEVICE_STATIC_REGIST("gd25q64", &GD25Q64, GD25Q64_Config, GD25Q64);


/*  */
#define FLASH_DRV_NAME       "25q64->flash"
static FW_Flash_Type EFlash;

static void FW_Flash_Config(void *dev)
{
    SNOR_Driver_Type *drv = FW_Driver_Find(FLASH_DRV_NAME);
    SNOR_Type *snor = FW_Device_Find("gd25q64");
    
    FW_Device_SetParent(dev, snor);
    FW_Device_SetDriver(dev, drv);
}
FW_DEVICE_STATIC_REGIST("eflash", &EFlash, FW_Flash_Config, EFlash);


/*  */
#define USB_DEV_NAME         "usb"
#define USB_DRV_NAME         "ll->usb"
static FW_USB_Type USB;

static void USB_Config(void *dev)
{
    FW_USB_Driver_Type *drv = FW_Driver_Find(USB_DRV_NAME);
    FW_Device_SetDriver(&USB, drv);
    USB.EN_Pin = PC1;
    USB.EN_VL = LEVEL_L;
}
FW_DEVICE_STATIC_REGIST(USB_DEV_NAME, &USB, USB_Config, USB);


void Test(void)
{
    u8 msg[U8_MAX + 1] = {0};
    u32 i;
    u32 addr;
    
    FW_Flash_Type *flash;
    FW_SPI_Type *spi;
    
    u16 VCC_EN = PC13;
    
    FW_USB_Type *usb;
    
    
    usb = FW_Device_Find(USB_DEV_NAME);
    if(usb == NULL)
    {
        while(1);
    }
    
    FW_System_Init();
    FW_Delay_Init();
    MM_Init(8 * 1024);
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    FW_Delay_Ms(1000);
    
    spi = FW_Device_Find(SNOR_SPI_NAME);
    if(spi == NULL)
    {
        while(1);
    }
    
    spi->CS_Pin = PA4;
    spi->SCK_Pin = PA5;
    spi->MISO_Pin = PA6;
    spi->MOSI_Pin = PA7;
    
    spi->Baudrate = 15000000;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
    
    flash = FW_Device_Find("eflash");
    if(flash == NULL)
    {
        while(1);
    }
    Flash_Init(flash);
    
    #if 1
    
    FW_USB_Init(usb);
    
    #else
    
    i = Flash_Get_CID(flash, msg, sizeof(msg));
    
    addr = 0;
    
    Flash_Erase_Sector(flash, 0);
    
    memset(msg, 0, sizeof(msg));
    Flash_Read(flash, addr, msg, sizeof(msg));
    
    for(i = 0; i < sizeof(msg); i++)
    {
        msg[i] = i + 3;
    }
    Flash_Write(flash, addr, msg, sizeof(msg));
    
    memset(msg, 0, sizeof(msg));
    Flash_Read(flash, addr, msg, sizeof(msg));
    
    #endif
    
    while(1);
}


#endif

