// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.11
// Project name: Pipboy_Project

#include "ui.h"

void ui_NFC_screen_init(void)
{
    ui_NFC = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NFC, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_NFC, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_NFC, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_NFC, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_NFC, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_NFC, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Button16 = lv_btn_create(ui_NFC);
    lv_obj_set_height(ui_Button16, 24);
    lv_obj_set_width(ui_Button16, lv_pct(100));
    lv_obj_set_x(ui_Button16, 0);
    lv_obj_set_y(ui_Button16, -108);
    lv_obj_set_align(ui_Button16, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button16, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button16, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Button16, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Button16, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Button16, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Button16, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Button16, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Button16, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label50 = lv_label_create(ui_Button16);
    lv_obj_set_width(ui_Label50, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label50, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label50, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label50, "RETURN");
    lv_obj_set_style_text_color(ui_Label50, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label50, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TextArea1 = lv_textarea_create(ui_NFC);
    lv_obj_set_width(ui_TextArea1, 275);
    lv_obj_set_height(ui_TextArea1, 70);
    lv_obj_set_x(ui_TextArea1, 0);
    lv_obj_set_y(ui_TextArea1, -54);
    lv_obj_set_align(ui_TextArea1, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextArea1, "Placeholder...");
    lv_obj_set_style_text_color(ui_TextArea1, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TextArea1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_TextArea1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TextArea1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TextArea1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_TextArea1, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_TextArea1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(ui_TextArea1, lv_color_hex(0x005900), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TextArea1, 255, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);

    ui_Keyboard1 = lv_keyboard_create(ui_NFC);
    lv_obj_set_width(ui_Keyboard1, 300);
    lv_obj_set_height(ui_Keyboard1, 120);
    lv_obj_set_x(ui_Keyboard1, 0);
    lv_obj_set_y(ui_Keyboard1, 200);
    lv_obj_set_align(ui_Keyboard1, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_Keyboard1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Keyboard1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Keyboard1, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Keyboard1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Keyboard1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_Keyboard1, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_Keyboard1, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Keyboard1, lv_color_hex(0xFFFFFF), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Keyboard1, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Keyboard1, lv_color_hex(0x33FF33), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Keyboard1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Keyboard1, 1, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_Keyboard1, lv_color_hex(0x33FF33), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Keyboard1, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Keyboard1, lv_color_hex(0xFFFFFF), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Keyboard1, 0, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui_Keyboard1, lv_color_hex(0x33FF33), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui_Keyboard1, 255, LV_PART_ITEMS | LV_STATE_CHECKED);

    ui_Button15 = lv_btn_create(ui_NFC);
    lv_obj_set_width(ui_Button15, 63);
    lv_obj_set_height(ui_Button15, 22);
    lv_obj_set_x(ui_Button15, 106);
    lv_obj_set_y(ui_Button15, -4);
    lv_obj_set_align(ui_Button15, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button15, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button15, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Button15, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Button15, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Button15, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Button15, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Button15, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Button15, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label49 = lv_label_create(ui_Button15);
    lv_obj_set_width(ui_Label49, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label49, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label49, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label49, "WRITE");
    lv_obj_set_style_text_color(ui_Label49, lv_color_hex(0x33FF33), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label49, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Button16, ui_event_Button16, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TextArea1, ui_event_TextArea1, LV_EVENT_ALL, NULL);
    lv_keyboard_set_textarea(ui_Keyboard1, ui_TextArea1);
    lv_obj_add_event_cb(ui_Keyboard1, ui_event_Keyboard1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Button15, ui_event_Button15, LV_EVENT_ALL, NULL);

}
