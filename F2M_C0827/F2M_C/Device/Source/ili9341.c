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

#include "fw_debug.h"
#include "fw_print.h"

#include "fw_bus.h"
#include "fw_gpio.h"
#include "fw_delay.h"


#define LCD_Delay_Ms(nms)    FW_Delay_Us(nms * 1000)


/* 系统总线驱动 */
__INLINE_STATIC_ void Bus_Init(LCD8_Type *dev)
{
    LCD8_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Init(dev);
}

__INLINE_STATIC_ void Bus_Write_Cmd(LCD8_Type *dev, const void *cmd)
{
    LCD8_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Write_Cmd(dev, cmd);
}

__INLINE_STATIC_ void Bus_Write_Data(LCD8_Type *dev, const void *pdata)
{
    LCD8_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Write_Data(dev, pdata);
}

__INLINE_STATIC_ void  Bus_Read_Data(LCD8_Type *dev, void *pdata)
{
    LCD8_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->Read_Data(dev, pdata);
}


/* 使用IO驱动 */
__INLINE_STATIC_ void IO_Init(LCD8_Type *dev)
{
    /* 所有IO口默认配置为输出 */
}

__INLINE_STATIC_ void IO_Write_Cmd(LCD8_Type *dev, const void *cmd)
{
    
}

__INLINE_STATIC_ void IO_Write_Data(LCD8_Type *dev, const void *pdata)
{

}

__INLINE_STATIC_ void IO_Read_Data(LCD8_Type *dev, void *pdata)
{
    /* 需要读取时，将IO口配置为输入，读取完成后，重新配置为输出 */
}


__INLINE_STATIC_ void LCD8_Write_Cmd(LCD8_Type *dev, u16 cmd)
{
    dev->Write_Cmd(dev, &cmd);
}

__INLINE_STATIC_ void LCD8_Write_Data(LCD8_Type *dev, u16 value)
{
    dev->Write_Data(dev, &value);
}

__INLINE_STATIC_ u16  LCD8_Read_Data(LCD8_Type *dev)
{
    u16 value;
    dev->Read_Data(dev, &value);
    return value;
}


/* ILI9341 For LCD Driver */
__INLINE_STATIC_ void ILI_Init(LCD_Type *dev)
{
    LCD8_Type *lcd8 = FW_Device_GetParent(dev);
    void *p, *q;
    
    p = FW_Device_GetDriver(lcd8);
    q = FW_Driver_Find("bus->lcd8");
    if(p && p == q)
    {
        /* 总线方式驱动 */
        lcd8->Init = Bus_Init;
        lcd8->Write_Cmd = Bus_Write_Cmd;
        lcd8->Write_Data = Bus_Write_Data;
        lcd8->Read_Data = Bus_Read_Data;
    }
    else
    {
        /* IO方式驱动 */
        lcd8->Init = IO_Init;
        lcd8->Write_Cmd = IO_Write_Cmd;
        lcd8->Write_Data = IO_Write_Data;
        lcd8->Read_Data = IO_Read_Data;
    }
    
    /* 初始化 */
    lcd8->Init(lcd8);
    
    /* 背光 */
    FW_GPIO_Init(lcd8->BL_Pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    FW_GPIO_CLR(lcd8->BL_Pin);
    
    /* 复位 */
    FW_GPIO_Init(lcd8->RST_Pin, FW_GPIO_Mode_Out_PPN, FW_GPIO_Speed_Low);
    FW_GPIO_CLR(lcd8->RST_Pin);
    LCD_Delay_Ms(200);
    FW_GPIO_SET(lcd8->RST_Pin);
    
    LCD8_Write_Cmd(lcd8, 0x00);
    LCD8_Write_Data(lcd8, 0x01);
    
    LCD8_Write_Cmd(lcd8, 0x01);
    LCD_Delay_Ms(20);
    LCD8_Write_Cmd(lcd8, 0x11);
    LCD_Delay_Ms(20);
    
    LCD8_Write_Cmd(lcd8, 0xEF);
    LCD8_Write_Data(lcd8, 0x03);
    LCD8_Write_Data(lcd8, 0x80);
    LCD8_Write_Data(lcd8, 0x02);
    
    /* 功耗控制B */
    LCD8_Write_Cmd(lcd8, 0xCF);
    LCD8_Write_Data(lcd8, 0x00);
    LCD8_Write_Data(lcd8, 0xBA);
    LCD8_Write_Data(lcd8, 0xB0);
    
    /* 电源序列控制 */
    LCD8_Write_Cmd(lcd8, 0xED);
    LCD8_Write_Data(lcd8, 0x67);
    LCD8_Write_Data(lcd8, 0x03);
    LCD8_Write_Data(lcd8, 0x12);
    LCD8_Write_Data(lcd8, 0x81);
    
    /* 驱动时序控制A */
    LCD8_Write_Cmd(lcd8, 0xE8);
    LCD8_Write_Data(lcd8, 0x85);
    LCD8_Write_Data(lcd8, 0x0A);
    LCD8_Write_Data(lcd8, 0x78);
    
    /* 功耗控制A */
    LCD8_Write_Cmd(lcd8, 0xCB);
    LCD8_Write_Data(lcd8, 0x39);
    LCD8_Write_Data(lcd8, 0x2C);
    LCD8_Write_Data(lcd8, 0x00);
    LCD8_Write_Data(lcd8, 0x34);
    LCD8_Write_Data(lcd8, 0x02);
    
    /* 泵比控制 */
    LCD8_Write_Cmd(lcd8, 0xF7);
    LCD8_Write_Data(lcd8, 0x20);
    
    /* 驱动时序控制B */
    LCD8_Write_Cmd(lcd8, 0xEA);
    LCD8_Write_Data(lcd8, 0x00);
    LCD8_Write_Data(lcd8, 0x00);
    
    /* 电源控制1 */
    LCD8_Write_Cmd(lcd8, 0xC0);
    LCD8_Write_Data(lcd8, 0x26);
    
    /* 电源控制2 */
    LCD8_Write_Cmd(lcd8, 0xC1);
    LCD8_Write_Data(lcd8, 0x10);
    
    /* VCM控制1 */
    LCD8_Write_Cmd(lcd8, 0xC5);
    LCD8_Write_Data(lcd8, 0x3F);    //3C or 3F
    LCD8_Write_Data(lcd8, 0x3C);
    
    LCD8_Write_Cmd(lcd8, 0x3A);
    LCD8_Write_Data(lcd8, 0x55);
    
    /* VCM控制2 */
    LCD8_Write_Cmd(lcd8, 0xC7);
    LCD8_Write_Data(lcd8, 0xBC);
    
    /* 存储访问控制 */
    LCD8_Write_Cmd(lcd8, 0x36);
    LCD8_Write_Data(lcd8, 0x48);
    
    /* 显示功能控制 */
    LCD8_Write_Cmd(lcd8, 0xB6);
    LCD8_Write_Data(lcd8, 0x0A);
    LCD8_Write_Data(lcd8, 0xA2);
    
    /* 禁止3D Gamma功能 */
    LCD8_Write_Cmd(lcd8, 0xF2);
    LCD8_Write_Data(lcd8, 0x00);
    
    /* Gamma curve selected */
    LCD8_Write_Cmd(lcd8, 0x26);
    LCD8_Write_Data(lcd8, 0x01);
    
    /* 设置Gamma1 */
    LCD8_Write_Cmd(lcd8, 0xE0);
    LCD8_Write_Data(lcd8, 0x0F);
    LCD8_Write_Data(lcd8, 0x1F);
    LCD8_Write_Data(lcd8, 0x1C);
    LCD8_Write_Data(lcd8, 0x0A);
    LCD8_Write_Data(lcd8, 0x0D);
    LCD8_Write_Data(lcd8, 0x06);
    LCD8_Write_Data(lcd8, 0x48);
    LCD8_Write_Data(lcd8, 0x76);
    LCD8_Write_Data(lcd8, 0x38);
    LCD8_Write_Data(lcd8, 0x08);
    LCD8_Write_Data(lcd8, 0x13);
    LCD8_Write_Data(lcd8, 0x06);
    LCD8_Write_Data(lcd8, 0x11);
    LCD8_Write_Data(lcd8, 0x0B);
    LCD8_Write_Data(lcd8, 0x08);
    
    /* 设置Gamma2 */
    LCD8_Write_Cmd(lcd8, 0xE1);
    LCD8_Write_Data(lcd8, 0x08);
    LCD8_Write_Data(lcd8, 0x22);
    LCD8_Write_Data(lcd8, 0x25);
    LCD8_Write_Data(lcd8, 0x04);
    LCD8_Write_Data(lcd8, 0x10);
    LCD8_Write_Data(lcd8, 0x04);
    LCD8_Write_Data(lcd8, 0x39);
    LCD8_Write_Data(lcd8, 0x23);
    LCD8_Write_Data(lcd8, 0x49);
    LCD8_Write_Data(lcd8, 0x02);
    LCD8_Write_Data(lcd8, 0x0A);
    LCD8_Write_Data(lcd8, 0x09);
    LCD8_Write_Data(lcd8, 0x30);
    LCD8_Write_Data(lcd8, 0x36);
    LCD8_Write_Data(lcd8, 0x0F);
    
    /* Exit Sleep */
    LCD8_Write_Cmd(lcd8, 0x11);
    LCD_Delay_Ms(50);
    
    /* Display On */
    LCD8_Write_Cmd(lcd8, 0x29);
    
    /* Direction Set */
    LCD8_Write_Cmd(lcd8, 0x36);
//    if(lcd8->Direct == LCD_Direct_Hori)
//    {
//        LCD8_Write_Cmd(lcd8, 0x36);
//        LCD8_Write_Data(lcd8, 0xAC);
//    }
//    else
//    {
//        LCD8_Write_Cmd(lcd8, 0x36);
//        LCD8_Write_Data(lcd8, 0xC9);
//    }
    if(lcd8->Rotate == LCD_Rotate_0)
    {
        LCD8_Write_Data(lcd8, 0xAC);
    }
    else if(lcd8->Rotate == LCD_Rotate_90)
    {
        LCD8_Write_Data(lcd8, 0xC9);
    }
    else if(lcd8->Rotate == LCD_Rotate_180)
    {
    
    }
    else
    {
    
    }
    
    /* Windows Set */
    LCD8_Write_Cmd(lcd8, 0x2A);
    LCD8_Write_Data(lcd8, 0);
    LCD8_Write_Data(lcd8, 0);
    LCD8_Write_Data(lcd8, lcd8->Pixel_W >> 8);
    LCD8_Write_Data(lcd8, lcd8->Pixel_W & 0xFF);
    
    LCD8_Write_Cmd(lcd8, 0x2B);
    LCD8_Write_Data(lcd8, 0);
    LCD8_Write_Data(lcd8, 0);
    LCD8_Write_Data(lcd8, lcd8->Pixel_H >> 8);
    LCD8_Write_Data(lcd8, lcd8->Pixel_H & 0xFF);
    
    LCD8_Write_Cmd(lcd8, 0x2C);
}

__INLINE_STATIC_ u32  ILI_Get_CID(LCD_Type *dev, u8 *cid)
{
    LCD8_Type *p = FW_Device_GetParent(dev);
    
    LCD8_Write_Cmd(p, 0xD3);
    cid[0] = LCD8_Read_Data(p);
    cid[1] = LCD8_Read_Data(p);
    cid[2] = LCD8_Read_Data(p);
    cid[3] = LCD8_Read_Data(p);
    
    return 4;
}

__INLINE_STATIC_ void ILI_BL_Set(LCD_Type *dev, u8 bright)
{
    LCD8_Type *p = FW_Device_GetParent(dev);
    FW_GPIO_SET(p->BL_Pin);
}

__INLINE_STATIC_ u8   ILI_BL_Get(LCD_Type *dev)
{
    return 0;
}

__INLINE_STATIC_ void ILI_Set_Cursor(LCD_Type *dev, u16 x0, u16 y0)
{
    LCD8_Type *parent = FW_Device_GetParent(dev);
    
    LCD8_Write_Cmd(parent, 0x2A);
    LCD8_Write_Data(parent, x0 >> 8);
    LCD8_Write_Data(parent, x0 & 0xFF);
    
    LCD8_Write_Cmd(parent, 0x2B);
    LCD8_Write_Data(parent, y0 >> 8);
    LCD8_Write_Data(parent, y0 & 0xFF);
}

__INLINE_STATIC_ void ILI_Fill_Area(LCD_Type *dev, u16 x0, u16 y0, u16 x1, u16 y1, u16 *color)
{
    LCD8_Type *p = FW_Device_GetParent(dev);
    
    #if 0
    u16 i, j, k;
    u16 width = x1 - x0 + 1;
    
    for(i = y0; i <= y1; i++)
    {
        LCD_Set_Cursor(dev, x0, i);
        LCD8_Write_Cmd(p, 0x2C);
        k = i - y0;
        for(j = 0; j < width; j++)
        {
            LCD8_Write_Data(p, color[k * width + j]);
        }
    }
    #else
    u16 i, j;
    u16 width = x1 - x0 + 1;
    u16 height = y1 - y0 + 1;
    
    LCD8_Write_Cmd(p, 0x2A);
    LCD8_Write_Data(p, x0 >> 8);
    LCD8_Write_Data(p, x0 & 0xFF);
    LCD8_Write_Data(p, x1 >> 8);
    LCD8_Write_Data(p, x1 & 0xFF);
    
    LCD8_Write_Cmd(p, 0x2B);
    LCD8_Write_Data(p, y0 >> 8);
    LCD8_Write_Data(p, y0 & 0xFF);
    LCD8_Write_Data(p, y1 >> 8);
    LCD8_Write_Data(p, y1 & 0xFF);
    
    LCD8_Write_Cmd(p, 0x2C);
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            LCD8_Write_Data(p, color[i * width + j]);
        }
    }
    #endif
}

__INLINE_STATIC_ void ILI_Draw_Point(LCD_Type *dev, u16 x0, u16 y0, u16 color)
{
    LCD8_Type *parent = FW_Device_GetParent(dev);
    
    ILI_Set_Cursor(dev, x0, y0);
    LCD8_Write_Cmd(parent, 0x2C);
    LCD8_Write_Data(parent, color);
}

__INLINE_STATIC_ u32  ILI_Get_Point(LCD_Type *dev, u16 x0, u16 y0)
{
    LCD8_Type *parent = FW_Device_GetParent(dev);
    u16 r, g, b;
    
    ILI_Set_Cursor(dev, x0, y0);
    LCD8_Write_Cmd(parent, 0x2E);
    LCD8_Read_Data(parent);            //dummy
    r = LCD8_Read_Data(parent);        //ili9341读寄存器的返回格式
    b = LCD8_Read_Data(parent);
    g = (r & 0xFF) << 8;
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}




/* LCD驱动实例化(ili9341) */
__CONST_STATIC_ LCD_Driver_Type Driver =
{
    .Init       = ILI_Init,
    .Get_CID    = ILI_Get_CID,
    .BL_Set     = ILI_BL_Set,
    .BL_Get     = ILI_BL_Get,
    .Fill_Area  = ILI_Fill_Area,
    .Draw_Point = ILI_Draw_Point,
    .Get_Point  = ILI_Get_Point,
};
FW_DRIVER_REGIST("ili9341->lcd", &Driver, ILI9341);





#include "fw_debug.h"
#if MODULE_TEST && IO_DEVICE_TEST && ILI9341_TEST
#include "fw_device.h"
#include "fw_bus.h"
#include "lcd.h"


#define LCD_BUS_NAME         "lcd_bus"
#define LCD_HW_NAME          "ili9341"
#define LCD_DEV_NAME         "lcd1"


static FW_Bus_LCD_Type LCD_Bus;
static void LCD_Bus_Config(void *dev)
{
    FW_Bus_LCD_Type *lcd_bus = dev;
    FW_Bus_Type *parent = FW_Device_Find("exbus");
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("ll->bus_sram"));
    
    lcd_bus->Base = 0x60000000;
}
FW_DEVICE_STATIC_REGIST(LCD_BUS_NAME, &LCD_Bus, LCD_Bus_Config, LCD_Bus);

static LCD8_Type ILI9341;
static void ILI9341_Config(void *dev)
{
    LCD8_Type *lcd = dev;
    FW_Bus_LCD_Type *parent = FW_Device_Find(LCD_BUS_NAME);
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("bus->lcd8"));
    
    lcd->Pixel_W = 240;
    lcd->Pixel_H = 320;
    
    lcd->Data_Width = LCD_Data_Width_16Bits;
    lcd->IOC_RS = FW_Bus_IOC_A16;
    
    lcd->RST_Pin = PC12;
    lcd->BL_Pin = PA1;
}
FW_DEVICE_STATIC_REGIST(LCD_HW_NAME, &ILI9341, ILI9341_Config, ILI9341);

static LCD_Type LCD;
static void LCD_Config(void *dev)
{
    LCD8_Type *parent = FW_Device_Find(LCD_HW_NAME);
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("ili9341->lcd"));
}
FW_DEVICE_STATIC_REGIST(LCD_DEV_NAME, &LCD, LCD_Config, LCD);


#include "fw_flash.h"
#include "fw_spi.h"

#define SNOR_SPI_NAME        "spi0"
//#define SNOR_SPI_NAME        "sspi0"
static void SPI_Pre_Init(void *pdata)
{
    FW_SPI_Type *spi = FW_Device_Find(SNOR_SPI_NAME);
    
    spi->CS_Pin = PA4;
    spi->SCK_Pin = PA5;
    spi->MISO_Pin = PA6;
    spi->MOSI_Pin = PA7;
    
    spi->Baudrate = 15000000;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
}
FW_PRE_INIT(SPI_Pre_Init, NULL);

static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(&SSPI0, drv);
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);


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




#include "lvgl.h"

#include "fw_gpio.h"
#include "fw_delay.h"

#include "fw_debug.h"

#include "lcd.h"

#include "board.h"


#include "lv_obj.h"
#include "lv_disp.h"
#include "lv_label.h"

#include "lv_port_disp.h"

#include "lv_color.h"


#include "gui_guider.h"
#include "events_init.h"


lv_ui guider_ui;


void lv_example_btn_1(void)
{
    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, NULL, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, NULL, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);
    label = lv_label_create(btn2);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);
}

void lv_example_btn_2(void)
{
    /*Init the style for the default state*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);
    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);
    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style, 8);
    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);
    
    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    
    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);
    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));
    
    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&style_pr, &trans);
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(btn1); /*Remove the style coming,→from the theme*/
    lv_obj_add_style(btn1, &style, 0);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn1);
    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
}


#include "ff.h"
#include "fw_usb.h"
#include "mm.h"

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
    u16 VCC_EN = PC13;
    u16 color;
    u32 cid;
    u32 argb;
    
    FW_Flash_Type *flash;
    FW_USB_Type *usb;
    
    LCD_Type *lcd = FW_Device_Find("lcd1");
    if(lcd == NULL)
    {
        while(1);
    }
    
//    MM_Init(6 * 1024);
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);

    #if 0
    
    flash = FW_Device_Find("eflash");
    if(flash == NULL)
    {
        while(1);
    }
    Flash_Init(flash);
    
    usb = FW_Device_Find(USB_DEV_NAME);
    if(usb == NULL)
    {
        while(1);
    }
    FW_USB_Init(usb);
    
    while(1);
    
    #else
    
    /* 文件系统 */
    FATFS fs;
    FIL file;
    FRESULT res;
    UINT value;
//    u8 FS_Buffer[FF_MAX_SS];
    
    /* 挂载的文件系统的存储器大小、扇区大小、数量需要保持一致 */
    res = f_mount(&fs, "0:", 1);
    if(res != FR_OK)
    {
//        res = f_mkfs("0:", 0, FS_Buffer, sizeof(FS_Buffer));
//        if(res)  while(1);
        
        f_mount(NULL, "0:", 1);
        
        res = f_mount(&fs, "0:", 1);
        if(res)  while(1);
    }
    
    /* UI */
    lv_init();
    
//    extern void lv_port_indev_init(void);
//    extern void lv_port_fs_init(void);
    
//    lv_port_indev_init();
//    lv_fs_fatfs_init();
    
    
//    LV_FONT_DECLARE(font_arial_16);
    LV_FONT_DECLARE(font_hczk_16);
    LV_FONT_DECLARE(font_alphc_16);
    
//    lv_obj_t *label = lv_label_create(lv_scr_act());
////    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
//    lv_obj_set_style_text_font(label, &font_arial_16, 0);
//    lv_obj_set_width(label, 80);
//    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
//    lv_obj_t *lab_h0 = lv_label_create(lv_scr_act());
//    lv_obj_set_style_text_font(lab_h0, &font_hczk_16, 0);
//    lv_obj_set_width(lab_h0, 80);
//    lv_obj_align(lab_h0, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_x(lab_h0, 0);
//    lv_obj_set_y(lab_h0, 30);
//    
//    lv_obj_t *lab_h1 = lv_label_create(lv_scr_act());
//    lv_obj_set_style_text_font(lab_h1, &font_alphc_16, 0);
//    lv_obj_set_width(lab_h1, 80);
//    lv_obj_align(lab_h1, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_x(lab_h1, 0);
//    lv_obj_set_y(lab_h1, 60);
    
    lv_obj_t *img_obj = lv_img_create(lv_scr_act());
//    lv_img_set_src(img_obj, "0:/image/mp1.png");
    lv_img_set_src(img_obj, "0:/image/mp1.bmp");
    lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_pos(img_obj, 0, 0);
    
//    res = f_open(&file, "0:/image/mp1.png", FA_READ);
//    if(res)
//    {
//        res = res;
//    }
    
    while(1)
    {
        lv_task_handler();
        
//        lv_obj_set_style_text_color(label, lv_color_make(0x00, 0x00, 0xFF), LV_STATE_DEFAULT);
//        lv_label_set_text(label, "滴速控制1");
        
//        lv_obj_set_style_text_color(lab_h0, lv_color_make(0x00, 0x00, 0xFF), LV_STATE_DEFAULT);
//        lv_label_set_text(lab_h0, "滴速控制2");
//        
//        lv_obj_set_style_text_color(lab_h1, lv_color_make(0x00, 0x00, 0xFF), LV_STATE_DEFAULT);
//        lv_label_set_text(lab_h1, "滴速控制3");
        
//        lv_img_set_src(img_obj, "0:/image/mp2.png");
        
        FW_Delay_Ms(10);
    }
    
    #endif
}


#endif

