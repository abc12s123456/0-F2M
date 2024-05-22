#include "lcd.h"


void LCD_Init(LCD_Type *dev)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

u32  LCD_Get_CID(LCD_Type *dev, u8 *cid)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_CID(dev, cid);
}

void LCD_BL_Set(LCD_Type *dev, u8 bright)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->BL_Set(dev, bright);
}

void LCD_Fill_Area(LCD_Type *dev, u16 x0, u16 y0, u16 x1, u16 y1, u16 *color)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Fill_Area(dev, x0, y0, x1, y1, color);
}

void LCD_Draw_Point(LCD_Type *dev, u16 x0, u16 y0, u16 color)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Draw_Point(dev, x0, y0, color);
}

u32  LCD_Get_Point(LCD_Type *dev, u16 x0, u16 y0)
{
    LCD_Driver_Type *drv = FW_Device_GetDriver(dev);
    return drv->Get_Point(dev, x0, y0);
}









