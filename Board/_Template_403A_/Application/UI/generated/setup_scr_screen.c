/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 320, 480);
	lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_meter_1
	ui->screen_meter_1 = lv_meter_create(ui->screen);
	// add scale screen_meter_1_scale_0
	lv_meter_scale_t *screen_meter_1_scale_0 = lv_meter_add_scale(ui->screen_meter_1);
	lv_meter_set_scale_ticks(ui->screen_meter_1, screen_meter_1_scale_0, 41, 2, 10, lv_color_hex(0xff0000));
	lv_meter_set_scale_major_ticks(ui->screen_meter_1, screen_meter_1_scale_0, 8, 5, 15, lv_color_hex(0xffff00), 10);
	lv_meter_set_scale_range(ui->screen_meter_1, screen_meter_1_scale_0, 0, 100, 300, 90);

	// add needle line for screen_meter_1_scale_0.
	ui->screen_meter_1_scale_0_ndline_0 = lv_meter_add_needle_line(ui->screen_meter_1, screen_meter_1_scale_0, 5, lv_color_hex(0x000000), -10);
	lv_meter_set_indicator_value(ui->screen_meter_1, ui->screen_meter_1_scale_0_ndline_0, 0);
	lv_obj_set_pos(ui->screen_meter_1, 30, 100);
	lv_obj_set_size(ui->screen_meter_1, 200, 200);

	//Write style for screen_meter_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_meter_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_meter_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_meter_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_meter_1, 100, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_meter_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_meter_1, lv_color_hex(0xff0000), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_meter_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_meter_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for screen_meter_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_meter_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_meter_1, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_meter_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_arc_1
	ui->screen_arc_1 = lv_arc_create(ui->screen);
	lv_arc_set_mode(ui->screen_arc_1, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->screen_arc_1, 0, 100);
	lv_arc_set_bg_angles(ui->screen_arc_1, 135, 45);
	lv_arc_set_value(ui->screen_arc_1, 70);
	lv_arc_set_rotation(ui->screen_arc_1, 0);
	lv_obj_set_style_arc_rounded(ui->screen_arc_1, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->screen_arc_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->screen_arc_1, 80, 311);
	lv_obj_set_size(ui->screen_arc_1, 100, 100);

	//Write style for screen_arc_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_arc_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_arc_1, lv_color_hex(0xf6f6f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_arc_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->screen_arc_1, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_arc_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_arc_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_arc_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_arc_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->screen_arc_1, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_opa(ui->screen_arc_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->screen_arc_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_arc_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_arc_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_arc_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_arc_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->screen_arc_1, 5, LV_PART_KNOB|LV_STATE_DEFAULT);

    #if 0
	//Write codes screen_roller_1
	ui->screen_roller_1 = lv_roller_create(ui->screen);
	lv_roller_set_options(ui->screen_roller_1, "1 \n2 \n3 \n4 \n5 ", LV_ROLLER_MODE_NORMAL);
	lv_obj_set_pos(ui->screen_roller_1, 213, 109);
	lv_obj_set_width(ui->screen_roller_1, 100);

	//Write style for screen_roller_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->screen_roller_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_roller_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_roller_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_roller_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_roller_1, lv_color_hex(0x333333), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_roller_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_roller_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_roller_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_roller_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_roller_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_roller_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_roller_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_roller_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_roller_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_roller_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_roller_1, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_roller_1, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_roller_1, lv_color_hex(0x2195f6), LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_roller_1, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_roller_1, lv_color_hex(0xFFFFFF), LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_roller_1, &lv_font_montserratMedium_12, LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_roller_1, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_roller_1, LV_TEXT_ALIGN_CENTER, LV_PART_SELECTED|LV_STATE_DEFAULT);

	lv_roller_set_visible_row_count(ui->screen_roller_1, 5);
	//Write codes screen_carousel_1
//	ui->screen_carousel_1 = lv_carousel_create(ui->screen);
//	lv_carousel_set_element_width(ui->screen_carousel_1, 100);
//	lv_obj_set_size(ui->screen_carousel_1, 136, 81);
//	ui->screen_carousel_1_element_1 = lv_carousel_add_element(ui->screen_carousel_1, 0);
//	ui->screen_carousel_1_element_2 = lv_carousel_add_element(ui->screen_carousel_1, 1);
//	ui->screen_carousel_1_element_3 = lv_carousel_add_element(ui->screen_carousel_1, 2);
//	lv_obj_set_pos(ui->screen_carousel_1, 183, 399);
//	lv_obj_set_size(ui->screen_carousel_1, 136, 81);
//	lv_obj_set_scrollbar_mode(ui->screen_carousel_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_carousel_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_carousel_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_carousel_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_carousel_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_carousel_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_carousel_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_carousel_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_carousel_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_carousel_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_screen_carousel_1_extra_element_items_default
	static lv_style_t style_screen_carousel_1_extra_element_items_default;
	ui_init_style(&style_screen_carousel_1_extra_element_items_default);
	
	lv_style_set_bg_opa(&style_screen_carousel_1_extra_element_items_default, 0);
	lv_style_set_outline_width(&style_screen_carousel_1_extra_element_items_default, 0);
	lv_style_set_border_width(&style_screen_carousel_1_extra_element_items_default, 0);
	lv_style_set_radius(&style_screen_carousel_1_extra_element_items_default, 5);
	lv_style_set_shadow_width(&style_screen_carousel_1_extra_element_items_default, 0);
	lv_obj_add_style(ui->screen_carousel_1_element_3, &style_screen_carousel_1_extra_element_items_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_carousel_1_element_2, &style_screen_carousel_1_extra_element_items_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_style(ui->screen_carousel_1_element_1, &style_screen_carousel_1_extra_element_items_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_FOCUSED for &style_screen_carousel_1_extra_element_items_focused
	static lv_style_t style_screen_carousel_1_extra_element_items_focused;
	ui_init_style(&style_screen_carousel_1_extra_element_items_focused);
	
	lv_style_set_bg_opa(&style_screen_carousel_1_extra_element_items_focused, 0);
	lv_style_set_outline_width(&style_screen_carousel_1_extra_element_items_focused, 0);
	lv_style_set_border_width(&style_screen_carousel_1_extra_element_items_focused, 0);
	lv_style_set_radius(&style_screen_carousel_1_extra_element_items_focused, 5);
	lv_style_set_shadow_width(&style_screen_carousel_1_extra_element_items_focused, 0);
	lv_obj_add_style(ui->screen_carousel_1_element_3, &style_screen_carousel_1_extra_element_items_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
	lv_obj_add_style(ui->screen_carousel_1_element_2, &style_screen_carousel_1_extra_element_items_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
	lv_obj_add_style(ui->screen_carousel_1_element_1, &style_screen_carousel_1_extra_element_items_focused, LV_PART_MAIN|LV_STATE_FOCUSED);

    #endif





	//The custom code of screen.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->screen);

	//Init events for screen.
	events_init_screen(ui);
}
