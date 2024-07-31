#include "lvgl.h"

#include "fw_gpio.h"
#include "fw_delay.h"

#include "fw_debug.h"


#if MODULE_TEST && UI_TEST
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


lv_ui guider_ui;


void Test(void)
{
    u16 VCC_EN = PC13;
    u16 color;
    u32 cid;
    u32 argb;
    
    LCD_Type *lcd = FW_Device_Find(LCD_DEV_NAME);
    if(lcd == NULL)
    {
        while(1);
    }
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
//    LCD_Init(lcd);
//    LCD_BL_Set(lcd, 100);
    
    lv_init();
    
    LCD_Get_CID(lcd, (u8 *)&cid);
    
//    color = 0x4040;
    color = RGB565(COLOR_GREY);
//    LCD_Fill_Area(lcd, 0, 0, 50, 50, &color);
    LCD_Draw_Point(lcd, 20, 20, color);
    
    cid = LCD_Get_Point(lcd, 20, 20);
    if(cid)
    {
        argb = argb;
    }
    
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    
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

    while(1)
    {
        lv_task_handler();
        
        color = 0xF800;
        LCD_Fill_Area(lcd, 50, 50, 100, 100, &color);
        
        FW_Delay_Ms(10);
    }
}


#endif

