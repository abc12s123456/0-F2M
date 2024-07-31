#include "lcd.h"

#include "fw_debug.h"
#include "fw_print.h"

#include "fw_spi.h"
#include "fw_gpio.h"
#include "fw_delay.h"


#define LCD_Delay_Ms(nms)    FW_Delay_Us(nms * 1000)


__INLINE_STATIC_ void SGP_EN(LCD8_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_CSSet(spi, Enable);
}

__INLINE_STATIC_ void SGP_DI(LCD8_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_CSSet(spi, Disable);
}

//__INLINE_STATIC_ void SGP_Write_Byte(LCD8_Type *dev, u8 value)
//{
//    FW_SPI_Type *spi = FW_Device_GetParent(dev);
//    FW_SPI_Write(spi, 0, &value, 1);
//}

//__INLINE_STATIC_ u8   SGP_Read_Byte(LCD8_Type *dev)
//{
//    FW_SPI_Type *spi = FW_Device_GetParent(dev);
//    u8 value;
//    FW_SPI_Read(spi, 0, &value, 1);
//    return value;
//}

#if 1

__INLINE_STATIC_ void LCD8_RS_Data(LCD8_Type *dev)
{
    FW_GPIO_SET(dev->RS_Pin);
}

__INLINE_STATIC_ void LCD8_RS_Cmd(LCD8_Type *dev)
{
    FW_GPIO_CLR(dev->RS_Pin);
}

#else

__INLINE_STATIC_ void LCD8_RS_Data(LCD8_Type *dev)
{
    FW_GPIO_CLR(dev->RS_Pin);
}

__INLINE_STATIC_ void LCD8_RS_Cmd(LCD8_Type *dev)
{
    FW_GPIO_SET(dev->RS_Pin);
}

#endif


__INLINE_STATIC_ void LCD8_Init(LCD8_Type *dev)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    FW_SPI_Init(spi);
}

__INLINE_STATIC_ void LCD8_Write_Cmd(LCD8_Type *dev, u8 cmd)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    SGP_EN(dev);
    LCD8_RS_Cmd(dev);
    FW_SPI_Write(spi, 0, &cmd, 1);
    SGP_DI(dev);
}

__INLINE_STATIC_ void LCD8_Write_Data(LCD8_Type *dev, u8 value)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    SGP_EN(dev);
    LCD8_RS_Data(dev);
    FW_SPI_Write(spi, 0, &value, 1);
    SGP_DI(dev);
}

__INLINE_STATIC_ void LCD8_Write(LCD8_Type *dev, u8 *pdata, u32 num)
{
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    SGP_EN(dev);
    LCD8_RS_Data(dev);
    FW_SPI_Write(spi, 0, pdata, num);
    SGP_DI(dev);
}


__INLINE_STATIC_ void ST7735_Init(LCD_Type *lcd)
{
    LCD8_Type *dev = FW_Device_GetParent(lcd);
    void *drv = FW_Device_GetDriver(dev);
    
    if(drv != FW_Driver_Find("spi->dev"))
    {
        LOG_E("无效驱动\r\n");
        while(1);
    }
    
    /* IO初始化 */
    if(dev->RST_Pin)
    {
        if(dev->RST_VL == LEVEL_L)
        {
            FW_GPIO_Init(dev->RST_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            FW_GPIO_SET(dev->RST_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->RST_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            FW_GPIO_CLR(dev->RST_Pin);
        }
    }
    
    if(dev->BL_Pin)
    {
        if(dev->BL_VL)
        {
            FW_GPIO_Init(dev->BL_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            FW_GPIO_SET(dev->BL_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->BL_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            FW_GPIO_CLR(dev->BL_Pin);
        }
    }
    
    FW_GPIO_Init(dev->RS_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    
    /* 端口初始化 */
    LCD8_Init(dev);
    
    if(dev->RST_VL == LEVEL_L)
    {
        FW_GPIO_CLR(dev->RST_Pin);
        LCD_Delay_Ms(100);
        FW_GPIO_SET(dev->RST_Pin);
    }
    else
    {
        FW_GPIO_SET(dev->RST_Pin);
        LCD_Delay_Ms(100);
        FW_GPIO_CLR(dev->RST_Pin);
    }
    LCD_Delay_Ms(100);
    
    LCD8_Write_Cmd(dev, 0x11);
    LCD_Delay_Ms(100);
    
    #if 1
    //Frame Rate
    LCD8_Write_Cmd(dev, 0xB1);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x2C);
    LCD8_Write_Data(dev, 0x2D);
    
    LCD8_Write_Cmd(dev, 0xB2);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x2C);
    LCD8_Write_Data(dev, 0x2D);
    
    LCD8_Write_Cmd(dev, 0xB3);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x2C);
    LCD8_Write_Data(dev, 0x2D);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x2C);
    LCD8_Write_Data(dev, 0x2D);
    
    //Column inversion 
    LCD8_Write_Cmd(dev, 0xB4);
    LCD8_Write_Data(dev, 0x07);
    
    //Power Sequence
    LCD8_Write_Cmd(dev, 0xC0);
    LCD8_Write_Data(dev, 0xA2);
    LCD8_Write_Data(dev, 0x02);
    LCD8_Write_Data(dev, 0x84);
    
    LCD8_Write_Cmd(dev, 0xC1);
    LCD8_Write_Data(dev, 0xC5);
    
    LCD8_Write_Cmd(dev, 0xC2);
    LCD8_Write_Data(dev, 0x0A);
    LCD8_Write_Data(dev, 0x00);
    
    LCD8_Write_Cmd(dev, 0xC3);
    LCD8_Write_Data(dev, 0x8A);
    LCD8_Write_Data(dev, 0x2A);
    
    LCD8_Write_Cmd(dev, 0xC4);
    LCD8_Write_Data(dev, 0x8A);
    LCD8_Write_Data(dev, 0xEE);
    
    //VCOM
    LCD8_Write_Cmd(dev, 0xC5);
    LCD8_Write_Data(dev, 0x0E);
    
    //MX, MY, RGB mode
    LCD8_Write_Cmd(dev, 0x36);
    LCD8_Write_Data(dev, 0xC8);
    
	//Gamma Sequence
    LCD8_Write_Cmd(dev, 0xE0);
    LCD8_Write_Data(dev, 0x0F);
    LCD8_Write_Data(dev, 0x1A);
    LCD8_Write_Data(dev, 0x0F);
    LCD8_Write_Data(dev, 0x18);
    LCD8_Write_Data(dev, 0x2F);
    LCD8_Write_Data(dev, 0x28);
    LCD8_Write_Data(dev, 0x20);
    LCD8_Write_Data(dev, 0x22);
    LCD8_Write_Data(dev, 0x1F);
    LCD8_Write_Data(dev, 0x1B);
    LCD8_Write_Data(dev, 0x23);
    LCD8_Write_Data(dev, 0x37);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x07);
    LCD8_Write_Data(dev, 0x02);
    LCD8_Write_Data(dev, 0x10);
    
    LCD8_Write_Cmd(dev, 0xE1);
    LCD8_Write_Data(dev, 0x0F);
    LCD8_Write_Data(dev, 0x1B);
    LCD8_Write_Data(dev, 0x0F);
    LCD8_Write_Data(dev, 0x17);
    LCD8_Write_Data(dev, 0x33);
    LCD8_Write_Data(dev, 0x2C);
    LCD8_Write_Data(dev, 0x29);
    LCD8_Write_Data(dev, 0x2E);
    LCD8_Write_Data(dev, 0x30);
    LCD8_Write_Data(dev, 0x30);
    LCD8_Write_Data(dev, 0x39);
    LCD8_Write_Data(dev, 0x3F);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x07);
    LCD8_Write_Data(dev, 0x03);
    LCD8_Write_Data(dev, 0x10);
    
    LCD8_Write_Cmd(dev, 0x2A);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x7F);
    
    LCD8_Write_Cmd(dev, 0x2B);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x9F);
    
    //Enable test command  
    LCD8_Write_Cmd(dev, 0xF0);
    LCD8_Write_Data(dev, 0x01);
    
    //Disable ram power save mode 
    LCD8_Write_Cmd(dev, 0xF6);
    LCD8_Write_Data(dev, 0x00);
    
    //65k mode
    LCD8_Write_Cmd(dev, 0x3A);
    LCD8_Write_Data(dev, 0x05);
    
    //Display on
    LCD8_Write_Cmd(dev, 0x29);
    #else
    //Frame Rate
    LCD8_Write_Cmd(dev, 0xB1);
    LCD8_Write_Data(dev, 0x05);
    LCD8_Write_Data(dev, 0x3C);
    LCD8_Write_Data(dev, 0x3C);
    
    LCD8_Write_Cmd(dev, 0xB2);
    LCD8_Write_Data(dev, 0x05);
    LCD8_Write_Data(dev, 0x3C);
    LCD8_Write_Data(dev, 0x3C);
    
    LCD8_Write_Cmd(dev, 0xB3);
    LCD8_Write_Data(dev, 0x05);
    LCD8_Write_Data(dev, 0x3C);
    LCD8_Write_Data(dev, 0x3C);
    LCD8_Write_Data(dev, 0x05);
    LCD8_Write_Data(dev, 0x3C);
    LCD8_Write_Data(dev, 0x3C);
    
    //Column inversion 
    LCD8_Write_Cmd(dev, 0xB4);
    LCD8_Write_Data(dev, 0x03);
    
    //Power Sequence
    LCD8_Write_Cmd(dev, 0xC0);
    LCD8_Write_Data(dev, 0x28);
    LCD8_Write_Data(dev, 0x08);
    LCD8_Write_Data(dev, 0x04);
    
    LCD8_Write_Cmd(dev, 0xC1);
    LCD8_Write_Data(dev, 0xC0);
    
    LCD8_Write_Cmd(dev, 0xC2);
    LCD8_Write_Data(dev, 0x0D);
    LCD8_Write_Data(dev, 0x00);
    
    LCD8_Write_Cmd(dev, 0xC3);
    LCD8_Write_Data(dev, 0x8D);
    LCD8_Write_Data(dev, 0x2A);
    
    LCD8_Write_Cmd(dev, 0xC4);
    LCD8_Write_Data(dev, 0x8D);
    LCD8_Write_Data(dev, 0xEE);
    
    //VCOM
    LCD8_Write_Cmd(dev, 0xC5);
    LCD8_Write_Data(dev, 0x1A);
    
    //MX, MY, RGB mode
//    LCD8_Write_Cmd(dev, 0x36);
//    LCD8_Write_Data(dev, 0xC8);
    
	//Gamma Sequence
    LCD8_Write_Cmd(dev, 0xE0);
    LCD8_Write_Data(dev, 0x04);
    LCD8_Write_Data(dev, 0x22);
    LCD8_Write_Data(dev, 0x07);
    LCD8_Write_Data(dev, 0x0A);
    LCD8_Write_Data(dev, 0x2E);
    LCD8_Write_Data(dev, 0x30);
    LCD8_Write_Data(dev, 0x25);
    LCD8_Write_Data(dev, 0x2A);
    LCD8_Write_Data(dev, 0x28);
    LCD8_Write_Data(dev, 0x26);
    LCD8_Write_Data(dev, 0x2E);
    LCD8_Write_Data(dev, 0x3A);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x03);
    LCD8_Write_Data(dev, 0x13);
    
    LCD8_Write_Cmd(dev, 0xE1);
    LCD8_Write_Data(dev, 0x04);
    LCD8_Write_Data(dev, 0x16);
    LCD8_Write_Data(dev, 0x06);
    LCD8_Write_Data(dev, 0x0D);
    LCD8_Write_Data(dev, 0x2D);
    LCD8_Write_Data(dev, 0x26);
    LCD8_Write_Data(dev, 0x23);
    LCD8_Write_Data(dev, 0x27);
    LCD8_Write_Data(dev, 0x27);
    LCD8_Write_Data(dev, 0x25);
    LCD8_Write_Data(dev, 0x2D);
    LCD8_Write_Data(dev, 0x3B);
    LCD8_Write_Data(dev, 0x00);
    LCD8_Write_Data(dev, 0x01);
    LCD8_Write_Data(dev, 0x04);
    LCD8_Write_Data(dev, 0x13);
    
//    LCD8_Write_Cmd(dev, 0x2A);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x7F);
//    
//    LCD8_Write_Cmd(dev, 0x2B);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x00);
//    LCD8_Write_Data(dev, 0x9F);
//    
//    //Enable test command  
//    LCD8_Write_Cmd(dev, 0xF0);
//    LCD8_Write_Data(dev, 0x01);
//    
//    //Disable ram power save mode 
//    LCD8_Write_Cmd(dev, 0xF6);
//    LCD8_Write_Data(dev, 0x00);
    
    //65k mode
    LCD8_Write_Cmd(dev, 0x3A);
    LCD8_Write_Data(dev, 0x05);
    
    //Display on
    LCD8_Write_Cmd(dev, 0x29);
    #endif
    
    //Direction Set
    LCD8_Write_Cmd(dev, 0x36);
    if(dev->Rotate == LCD_Rotate_0)
    {
        LCD8_Write_Data(dev, 0xC8);
    }
    else if(dev->Rotate == LCD_Rotate_90)
    {
        LCD8_Write_Data(dev, 0xA8);
    }
    else if(dev->Rotate == LCD_Rotate_180)
    {
        LCD8_Write_Data(dev, 0x08);
    }
    else
    {
        LCD8_Write_Data(dev, 0x68);
    }
    
    // Windows Set
//    dev->Pixel_W += dev->X_Offset;
//    dev->Pixel_H += dev->Y_Offset;
    
    LCD8_Write_Cmd(dev, 0x2A);
    LCD8_Write_Data(dev, dev->X_Offset >> 8);
    LCD8_Write_Data(dev, dev->X_Offset & 0xFF);
    LCD8_Write_Data(dev, dev->Pixel_W >> 8);
    LCD8_Write_Data(dev, dev->Pixel_W & 0xFF);
    
    LCD8_Write_Cmd(dev, 0x2B);
    LCD8_Write_Data(dev, dev->Y_Offset >> 8);
    LCD8_Write_Data(dev, dev->Y_Offset & 0xFF);
    LCD8_Write_Data(dev, dev->Pixel_H >> 8);
    LCD8_Write_Data(dev, dev->Pixel_H & 0xFF);
    
    LCD8_Write_Cmd(dev, 0x2C);
}

__INLINE_STATIC_ u32  ST7735_Get_CID(LCD_Type *lcd, u8 *cid)
{
    return 0;
}

__INLINE_STATIC_ void ST7735_BL_Set(LCD_Type *lcd, u8 bright)
{
    
}

__INLINE_STATIC_ u8   ST7735_BL_Get(LCD_Type *lcd)
{
    return 0;
}

__INLINE_STATIC_ void ST7735_Set_Cursor(LCD_Type *lcd, u16 x0, u16 y0)
{
    LCD8_Type *dev = FW_Device_GetParent(lcd);
    
    x0 += dev->X_Offset;
    y0 += dev->Y_Offset;
    
    LCD8_Write_Cmd(dev, 0x2A);
    LCD8_Write_Data(dev, x0 >> 8);
    LCD8_Write_Data(dev, x0 & 0xFF);
    
    LCD8_Write_Cmd(dev, 0x2B);
    LCD8_Write_Data(dev, y0 >> 8);
    LCD8_Write_Data(dev, y0 & 0xFF);
    
    LCD8_Write_Cmd(dev, 0x2C);
}

//__INLINE_STATIC_ void ST7735_Clear(LCD_Type *lcd, u16 color)
//{
//    
//}

__INLINE_STATIC_ void ST7735_Fill_Area(LCD_Type *lcd, u16 x0, u16 y0, u16 x1, u16 y1, u16 *color)
{
    LCD8_Type *dev = FW_Device_GetParent(lcd);
    u8 msg[4];
    
    x0 += dev->X_Offset, x1 += dev->X_Offset;
    y0 += dev->Y_Offset, y1 += dev->Y_Offset;
    
    #if 0
    u16 i, j, k;
    u16 width = x1 - x0 + 1;
    u16 c;
    
    for(i = y0; i <= y1; i++)
    {
        ST7735_Set_Cursor(lcd, x0, i);
        k = i - y0;
        for(j = 0; j < width; j++)
        {
            c = color[k * width + j];
            LCD8_Write_Data(dev, c >> 8);
            LCD8_Write_Data(dev, c & 0xFF);
        }
    }
    #else
    u16 i, j;
    u16 width = x1 - x0 + 1;
    u16 height = y1 - y0 + 1;
    u16 c;
    
    LCD8_Write_Cmd(dev, 0x2A);
    msg[0] = x0 >> 8;
    msg[1] = x0 & 0xFF;
    msg[2] = x1 >> 8;
    msg[3] = x1 & 0xFF;
    LCD8_Write(dev, msg, 4);
    
    LCD8_Write_Cmd(dev, 0x2B);
    msg[0] = y0 >> 8;
    msg[1] = y0 & 0xFF;
    msg[2] = y1 >> 8;
    msg[3] = y1 & 0xFF;
    LCD8_Write(dev, msg, 4);
    
    LCD8_Write_Cmd(dev, 0x2C);
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            c = color[i * width + j];
            msg[0] = c >> 8;
            msg[1] = c & 0xFF;
            LCD8_Write(dev, msg, 2);
        }
    }
    #endif
}

__INLINE_STATIC_ void ST7735_Draw_Point(LCD_Type *lcd, u16 x0, u16 y0, u16 color)
{
    LCD8_Type *dev = FW_Device_GetParent(lcd);
    
    ST7735_Set_Cursor(lcd, x0, y0);
    LCD8_Write_Data(dev, color >> 8);
    LCD8_Write_Data(dev, color & 0xFF);
}

__INLINE_STATIC_ u32  ST7735_Get_Point(LCD_Type *lcd, u16 x0, u16 y0)
{
    return 0;
}

__INLINE_STATIC_ Bool ST7735_Get_ScreenSize(LCD_Type *lcd)
{
    LCD8_Type *dev = FW_Device_GetParent(lcd);
    
    lcd->Width = dev->Pixel_W;
    lcd->Height = dev->Pixel_H;
    
    return True;
}




/* LCD驱动实例化(st7735) */
__CONST_STATIC_ LCD_Driver_Type LCD_Driver =
{
    .Init       = ST7735_Init,
    .Get_CID    = ST7735_Get_CID,
    .BL_Set     = ST7735_BL_Set,
    .BL_Get     = ST7735_BL_Get,
    .Fill_Area  = ST7735_Fill_Area,
    .Draw_Point = ST7735_Draw_Point,
    .Get_Point  = ST7735_Get_Point,
    .Get_Screen_Size = ST7735_Get_ScreenSize,
};
FW_DRIVER_REGIST("st7735->lcd", &LCD_Driver, ST7735);







#include "fw_debug.h"
#if MODULE_TEST && SPI_DEVICE_TEST && ST7735_TEST


static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Type *spi = dev;
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(spi, drv);
    
    spi->CS_Pin = PB12;
    spi->SCK_Pin = PB13;
    spi->MOSI_Pin = PB15;
    
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
    spi->Data_Width = FW_SPI_DataWidth_8Bits;
    spi->CS_VL = LEVEL_L;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);

static LCD8_Type ST7735;
static void ST7735_Config(void *dev)
{
    LCD8_Type *lcd = dev;
    FW_SPI_Type *spi = FW_Device_Find("sspi0");
    
    FW_Device_SetParent(lcd, spi);
    FW_Device_SetDriver(lcd, FW_Driver_Find("spi->dev"));
    
    lcd->Rotate = LCD_Rotate_0;
    lcd->Pixel_W = 80;
    lcd->Pixel_H = 160;
    lcd->X_Offset = 24;
    lcd->Y_Offset = 0;
    
    lcd->Data_Width = LCD_Data_Width_8Bits;
    
    lcd->RST_Pin = PB14;
    lcd->RS_Pin = PD8;
}
FW_DEVICE_STATIC_REGIST("st7735", &ST7735, ST7735_Config, _ST7735);


static LCD_Type LCD;
static void LCD_Config(void *dev)
{
    LCD8_Type *parent = FW_Device_Find("st7735");
    
    FW_Device_SetParent(dev, parent);
    FW_Device_SetDriver(dev, FW_Driver_Find("st7735->lcd"));
}
FW_DEVICE_STATIC_REGIST("lcd", &LCD, LCD_Config, LCD1);


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


void Demo(void)
{
    lv_obj_t *obj = lv_label_create(lv_scr_act());
    lv_label_set_text(obj, "hellor world\r\n");
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
    lv_obj_set_width(obj, 200);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 5, 5);
//    lv_obj_set_style_text_color(obj,
    lv_obj_del(obj);
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


lv_ui guider_ui;


void Test(void)
{
    u16 VCC_EN = PB11;
    u16 color;
    u32 cid;
    u32 argb;
    
    LCD_Type *lcd = FW_Device_Find("lcd");
    if(lcd == NULL)
    {
        while(1);
    }
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
//    LCD_Init(lcd);
//    LCD_BL_Set(lcd, 100);
    
    lv_init();
    
//    LCD_Get_CID(lcd, (u8 *)&cid);
    
//    color = 0x4040;
    color = RGB565(COLOR_GREY);
//    LCD_Fill_Area(lcd, 0, 0, 50, 50, &color);
    LCD_Draw_Point(lcd, 20, 20, color);
    
    cid = LCD_Get_Point(lcd, 20, 20);
    if(cid)
    {
        argb = argb;
    }
    
//    setup_ui(&guider_ui);
//    events_init(&guider_ui);
    
//    lv_color_t bg_color = lv_color_make(0x40, 0, 0);
//    lv_obj_set_style_bg_color(lv_scr_act(), bg_color, 0);
    
    lv_obj_t *obj = lv_label_create(lv_scr_act());
//    lv_obj_set_style_text_color(obj, lv_palette_darken(LV_PALETTE_GREY, 1), LV_PART_TICKS);
    lv_label_set_text(obj, "hellor world");
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
    lv_obj_set_width(obj, 200);
    lv_obj_align(obj, LV_ALIGN_CENTER, 150, 150);
    
//    static lv_style_t style;
//    lv_style_init(&style);
//    lv_style_set_text_font(&style, &

    lv_example_btn_2();
    
//    LV_IMG_DECLARE(PowerUpUI);
    
//    lv_obj_t *img = lv_img_create(lv_scr_act());
//    lv_img_set_src(img, &PowerUpUI);
//    lv_obj_center(img);
    
    while(1)
    {
        lv_task_handler();
        
//        color = 0xF800;
//        LCD_Fill_Area(lcd, 50, 50, 70, 100, &color);
        
//        LCD_Fill_Area(lcd, 0, 0, 50, 100, &color);
        
        FW_Delay_Ms(10);
    }
}


#endif

