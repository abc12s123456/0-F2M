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









