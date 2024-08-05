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
#include "fw_bus.h"

#include "fw_debug.h"
#include "fw_print.h"

#include "lcd.h"






/* 8080接口LCD，父设备：LCD_Bus */
__INLINE_STATIC_ void Bus_LCD8_WriteCmdU32(FW_Bus_LCD_Type *bus, const void *cmd)
{
    *(__IO_ u32 *)(bus->Base) = *(u32 *)cmd;
}

__INLINE_STATIC_ void Bus_LCD8_WriteCmdU16(FW_Bus_LCD_Type *bus, const void *cmd)
{
    *(__IO_ u16 *)(bus->Base) = *(u16 *)cmd;
}

__INLINE_STATIC_ void Bus_LCD8_WriteCmdU8(FW_Bus_LCD_Type *bus, const void *cmd)
{
    *(__IO_ u8 *)(bus->Base) = *(u8 *)cmd;
}

__INLINE_STATIC_ void Bus_LCD8_WriteDataU32(FW_Bus_LCD_Type *bus, const void *pdata)
{
    *(__IO_ u32 *)(bus->Base | (1 << (bus->IOC_RS + 1))) = *(u32 *)pdata;
}

__INLINE_STATIC_ void Bus_LCD8_WriteDataU16(FW_Bus_LCD_Type *bus, const void *pdata)
{
    *(__IO_ u16 *)(bus->Base | (1 << (bus->IOC_RS + 1))) = *(u16 *)pdata;
}

__INLINE_STATIC_ void Bus_LCD8_WriteDataU8(FW_Bus_LCD_Type *bus, const void *pdata)
{
    *(__IO_ u8 *)(bus->Base | (1 << (bus->IOC_RS + 1))) = *(u8 *)pdata;
}

__INLINE_STATIC_ void Bus_LCD8_ReadDataU32(FW_Bus_LCD_Type *bus, void *pdata)
{
    *(u32 *)pdata = *(__IO_ u32 *)(bus->Base | (1 << (bus->IOC_RS + 1)));
}

__INLINE_STATIC_ void Bus_LCD8_ReadDataU16(FW_Bus_LCD_Type *bus, void *pdata)
{
    *(u16 *)pdata = *(__IO_ u16 *)(bus->Base | (1 << (bus->IOC_RS + 1)));
}

__INLINE_STATIC_ void Bus_LCD8_ReadDataU8(FW_Bus_LCD_Type *bus, void *pdata)
{
    *(u8 *)pdata = *(__IO_ u8 *)(bus->Base | (1 << (bus->IOC_RS + 1)));
}


/* LCD8驱动 */
__INLINE_STATIC_ void Bus_LCD8_Init(LCD8_Type *dev)
{
    FW_Bus_LCD_Type *p = FW_Device_GetParent(dev);
    Bus_LCD_Driver_Type *drv;
    
    if(dev->Data_Width == LCD_Data_Width_32Bits)
    {
        p->Bus_Width = FW_Bus_Width_32Bits;
        
        p->Write_Cmd = Bus_LCD8_WriteCmdU32;
        p->Write_Data = Bus_LCD8_WriteDataU32;
        p->Read_Data = Bus_LCD8_ReadDataU32;
    }
    else if(dev->Data_Width == LCD_Data_Width_16Bits)
    {
        p->Bus_Width = FW_Bus_Width_16Bits;
        
        p->Write_Cmd = Bus_LCD8_WriteCmdU16;
        p->Write_Data = Bus_LCD8_WriteDataU16;
        p->Read_Data = Bus_LCD8_ReadDataU16;
    }
    else if(dev->Data_Width == LCD_Data_Width_8Bits)
    {
        p->Bus_Width = FW_Bus_Width_8Bits;
        
        p->Write_Cmd = Bus_LCD8_WriteCmdU8;
        p->Write_Data = Bus_LCD8_WriteDataU8;
        p->Read_Data = Bus_LCD8_ReadDataU8;
    }
    else
    {
        /* 未设定传输数据位宽 */
        while(1);
    }
    
    /* LCD-RS使用的地址线 */
    p->IOC_RS = dev->IOC_RS;
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    
    /*  */
    drv->Init(p);
}

__INLINE_STATIC_ void Bus_LCD8_WriteCmd(LCD8_Type *dev, const void *cmd)
{
    FW_Bus_LCD_Type *p = FW_Device_GetParent(dev);
    p->Write_Cmd(p, cmd);
}

__INLINE_STATIC_ void Bus_LCD8_WriteData(LCD8_Type *dev, const void *pdata)
{
    FW_Bus_LCD_Type *p = FW_Device_GetParent(dev);
    p->Write_Data(p, pdata);
}

__INLINE_STATIC_ void Bus_LCD8_ReadData(LCD8_Type *dev, void *pdata)
{
    FW_Bus_LCD_Type *p = FW_Device_GetParent(dev);
    p->Read_Data(p, pdata);
}




/* 使用Bus驱动LCD8设备 */
__CONST_STATIC_ LCD8_Driver_Type LCD_Driver =
{
    .Init       = Bus_LCD8_Init,
    .Write_Cmd  = Bus_LCD8_WriteCmd,
    .Write_Data = Bus_LCD8_WriteData,
    .Read_Data  = Bus_LCD8_ReadData,
};
FW_DRIVER_REGIST("bus->lcd8", &LCD_Driver, AL_Bus_LCD);



#if 0
/* Reg */
__INLINE_STATIC_ void FW_Bus_WriteU32Reg(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u32 *)(dev->Base + addr) = *(u32 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU16Reg(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u16 *)(dev->Base + addr) = *(u16 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU8Reg(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u8 *)(dev->Base + addr) = *(u8 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_ReadU32Reg(FW_Bus_Type *dev, void *pdata)
{
    *(u32 *)pdata = *(__IO_ u32 *)(dev->Base + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU16Reg(FW_Bus_Type *dev, void *pdata)
{
    *(u16 *)pdata = *(__IO_ u16 *)(dev->Base + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU8Reg(FW_Bus_Type *dev, void *pdata)
{
    *(u8 *)pdata = *(__IO_ u8 *)(dev->Base + addr);
}

/* Data */
__INLINE_STATIC_ void FW_Bus_WriteU32Data(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr) = *(u32 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU16Data(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr) = *(u16 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU8Data(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr) = *(u8 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_ReadU32Data(FW_Bus_Type *dev, void *pdata)
{
    *(u32 *)pdata = *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU16Data(FW_Bus_Type *dev, void *pdata)
{
    *(u16 *)pdata = *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU8Data(FW_Bus_Type *dev, void *pdata)
{
    *(u8 *)pdata = *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_RS + 1))) + addr);
}

/* Addr */
__INLINE_STATIC_ void FW_Bus_WriteU32Addr(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr) = *(u32 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU16Addr(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr) = *(u16 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU8Addr(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr) = *(u8 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_ReadU32Addr(FW_Bus_Type *dev, void *pdata)
{
    *(u32 *)pdata = *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU16Addr(FW_Bus_Type *dev, void *pdata)
{
    *(u16 *)pdata = *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU8Addr(FW_Bus_Type *dev, void *pdata)
{
    *(u8 *)pdata = *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_ALE + 1))) + addr);
}

/* Cmd */
__INLINE_STATIC_ void FW_Bus_WriteU32Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr) = *(u32 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU16Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr) = *(u16 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_WriteU8Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr) = *(u8 *)pdata;
}

__INLINE_STATIC_ void FW_Bus_ReadU32Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(u32 *)pdata = *(__IO_ u32 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU16Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(u16 *)pdata = *(__IO_ u16 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr);
}

__INLINE_STATIC_ void FW_Bus_ReadU8Cmd(FW_Bus_Type *dev, void *pdata)
{
    *(u8 *)pdata = *(__IO_ u8 *)((dev->Base | (1 << (dev->IOC_CLE + 1))) + addr);
}


void FW_Bus_Init(FW_Bus_Type *dev)
{
    FW_Bus_Driver_Type *drv;
    
    if(dev->Width == FW_Bus_Width_32Bits)
    {
        dev->WriteReg = FW_Bus_WriteU32Reg;
        dev->WriteData = FW_Bus_WriteU32Data;
        dev->WriteAddr = FW_Bus_WriteU32Addr;
        dev->WriteCmd = FW_Bus_WriteU32Cmd;
        
        dev->ReadReg = FW_Bus_ReadU32Reg;
        dev->ReadData = FW_Bus_ReadU32Data;
        dev->ReadAddr = FW_Bus_ReadU32Addr;
        dev->ReadCmd = FW_Bus_ReadU32Cmd;
        
        dev->Offset = sizeof(u32);
    }
    else if(dev->Width == FW_Bus_Width_16Bits)
    {
        dev->WriteReg = FW_Bus_WriteU16Reg;
        dev->WriteData = FW_Bus_WriteU16Data;
        dev->WriteAddr = FW_Bus_WriteU16Addr;
        dev->WriteCmd = FW_Bus_WriteU16Cmd;
        
        dev->ReadReg = FW_Bus_ReadU16Reg;
        dev->ReadData = FW_Bus_ReadU16Data;
        dev->ReadAddr = FW_Bus_ReadU16Addr;
        dev->ReadCmd = FW_Bus_ReadU16Cmd;
        
        dev->Offset = sizeof(u16);
    }
    else if(dev->Width == FW_Bus_Width_8Bits)
    {
        dev->WriteReg = FW_Bus_WriteU8Reg;
        dev->WriteData = FW_Bus_WriteU8Data;
        dev->WriteAddr = FW_Bus_WriteU8Addr;
        dev->WriteCmd = FW_Bus_WriteU8Cmd;
        
        dev->ReadReg = FW_Bus_ReadU8Reg;
        dev->ReadData = FW_Bus_ReadU8Data;
        dev->ReadAddr = FW_Bus_ReadU8Addr;
        dev->ReadCmd = FW_Bus_ReadU8Cmd;
        
        dev->Offset = sizeof(u8);
    }
    else
    {
        /* 未设定传输数据位宽 */
        while(1);
    }
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    
    drv->Init(dev);
}

void FW_Bus_WriteReg(FW_Bus_Type *dev, void *pdata)
{
    dev->WriteReg(dev, addr, pdata);
}

void FW_Bus_ReadReg(FW_Bus_Type *dev, void *pdata)
{
    dev->ReadReg(dev, addr, pdata);
}

void FW_Bus_WriteData(FW_Bus_Type *dev, void *pdata, u32 num)
{
    u8 *p = (u8 *)pdata;
    
    num /= dev->Offset;
    
    while(num--)
    {
        dev->WriteData(dev, addr, p);
        p += dev->Offset;
        addr += dev->Offset;
    }
}

void FW_Bus_ReadData(FW_Bus_Type *dev, void *pdata, u32 num)
{
    u8 *p = (u8 *)pdata;
    
    num /= dev->Offset;
    
    while(num--)
    {
        dev->ReadData(dev, addr, p);
        p += dev->Offset;
        addr += dev->Offset;
    }
}

//void FW_Bus_WriteAddr(FW_Bus_Type *dev, void *pdata)
    
#endif






