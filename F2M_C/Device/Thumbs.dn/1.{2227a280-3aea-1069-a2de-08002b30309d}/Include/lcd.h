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
 
#ifndef _LCD_H_
#define _LCD_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* LCD驱动编号 */
#define LCD_DRV_8080_NUM          0         //8080接口LCD驱动


#define RGB(r, g, b)              ((u32)((r << 16) | (g << 8) | b))
#define ARGB(a, r, g, b)          ((u32)((a << 24) | RGB(r, g, b)))

#define RGB565(rgb)(\
((((u8)(rgb >> 16)) >> 3) << 11) |\
((((u8)(rgb >> 8)) >> 2) << 5) |\
(((u8)rgb) >> 3))


/* 常用颜色的RGB编码 */
#define COLOR_BLACK               RGB(0, 0, 0)             //黑
#define COLOR_RED                 RGB(255, 0, 0)           //红
#define COLOR_WHITE               RGB(255, 255, 255)       //白
#define COLOR_YELLOW              RGB(255, 255, 0)         //黄
#define COLOR_BLUE                RGB(0, 0, 255)           //蓝
#define COLOR_GREEN               RGB(0, 255, 0)           //绿
#define COLOR_CYAN                RGB(0, 255, 255)         //青
#define COLOR_PURPLE              RGB(255, 0, 255)         //紫
#define COLOR_GREY                RGB(192, 192, 192)       //灰
#define COLOR_ORANGE              RGB(0xF4, 0x79, 0x20)    //橙


typedef enum
{
    LCD_Data_Width_8Bits,
    LCD_Data_Width_16Bits,
    LCD_Data_Width_32Bits,
}LCD_Data_Width_Enum;


typedef enum
{
    LCD_Direct_Vert,         //竖屏
    LCD_Direct_Hori,         //横屏
}LCD_Direct_Enum;


/* 屏幕旋转角度 */
typedef enum
{
    LCD_Rotate_0,
    LCD_Rotate_90,
    LCD_Rotate_180,
    LCD_Rotate_270,
}LCD_Rotate_Enum;


typedef struct 
{
    FW_Device_Type Device;
    
    u32 BL_Pin : 16;         //背光IO
    u32 BL_VL : 1;           //背光有效电平
    u32 Bright : 7;          //背光强度，0~100
    
    u32 : 8;
}LCD_Type;


typedef struct
{
    void (*Init)(LCD_Type *dev);
    
    u32  (*Get_CID)(LCD_Type *dev, u8 *cid);
    
    void (*BL_Set)(LCD_Type *dev, u8 bright);
    u8   (*BL_Get)(LCD_Type *dev);
    
    void (*Fill_Area)(LCD_Type *dev, u16 x0, u16 y0, u16 x1, u16 y1, u16 *color);
    void (*Draw_Point)(LCD_Type *dev, u16 x0, u16 y0, u16 color);
    
    u32  (*Get_Point)(LCD_Type *dev, u16 x0, u16 y0);
}LCD_Driver_Type;


void LCD_Init(LCD_Type *dev);

u32  LCD_Get_CID(LCD_Type *dev, u8 *cid);

void LCD_BL_Set(LCD_Type *dev, u8 bright);
u8   LCD_BL_Get(LCD_Type *dev);

void LCD_Fill_Area(LCD_Type *dev, u16 x0, u16 y0, u16 x1, u16 y1, u16 *color);
void LCD_Draw_Point(LCD_Type *dev, u16 x0, u16 y0, u16 color);
u32  LCD_Get_Point(LCD_Type *dev, u16 x0, u16 y0);




/* 8080接口LCD */
typedef struct LCD8 LCD8_Type;

struct LCD8
{
    FW_Device_Type Device;
    
    u16 *Data_Pin_Array;     //数据IO数组指针(使用IO驱动时使用)
    
    u32 CS_Pin : 16;         //片选
    u32 WR_Pin : 16;         //写使能
    u32 RD_Pin : 16;         //读使能
    u32 RS_Pin : 16;         //命令/数据
    
    u32 RST_Pin : 16;        //复位
    u32 BL_Pin : 16;         //背光
    
    u32 CID;                 //芯片ID
    
    u32 Data_Width : 2;      //数据位宽，参照LCD_Data_Width_Enum
    
    u32 Pixel_W : 10;        //宽度像素
    u32 Pixel_H : 10;        //高度像素
    
    u32 RST_VL : 1;          //复位有效电平
    u32 BL_VL : 1;           //背光有效电平
    u32 Bright : 7;          //背光强度，0~100
    
    u32 : 1;
    
    u32 IOC_RS : 6;          //参照FW_Bus_IOC_Enum
    
    u32 Rotate : 2;          //屏幕旋转角度，参照LCD_Rotate_Enum
    s32 X_Offset : 8;
    s32 Y_Offset : 8;
    
    u32 : 8;
    
    void (*Init)(LCD8_Type *dev);

    /* LCD8有多种驱动方式：Bus，IO，SPI等 */
    void (*Write_Cmd)(LCD8_Type *dev, const void *cmd);
    void (*Write_Data)(LCD8_Type *dev, const void *pdata);
    void (*Read_Data)(LCD8_Type *dev, void *pdata);
};


/* LCD8080通用驱动 */
typedef struct
{
    void (*Init)(LCD8_Type *dev);
    
    u32  (*Get_CID)(LCD8_Type *dev);
    
    void (*BL_Set)(LCD8_Type *dev, u8 bright);
    u8   (*BL_Get)(LCD8_Type *dev);
    
    void (*Write_Cmd)(LCD8_Type *dev, const void *cmd);
    void (*Write_Data)(LCD8_Type *dev, const void *pdata);
    void (*Read_Data)(LCD8_Type *dev, void *pdata);
}LCD8_Driver_Type;




///* SPI接口驱动 */
//typedef struct LCD_SPI LCD_SPI_Type;

//struct LCD_SPI
//{
//    FW_Device_Type Device;
//    
//    u32 RST_Pin : 16;
//    u32 BL_Pin : 16;
//    
//    u32 CID;
//    
//    u32 Data_Width : 2;
//    u32 Direct : 1;
//    
//    u32 Vert_Pixel : 10;
//    u32 Hori_Pixel : 10;
//    
//    u32 BL_VL : 1;           //背光有效电平
//    u32 Bright : 7;          //背光强度，0~100
//    
//    u32 : 1;
//    
//    void (*Init)(
//};



#ifdef __cplusplus
}
#endif

#endif

