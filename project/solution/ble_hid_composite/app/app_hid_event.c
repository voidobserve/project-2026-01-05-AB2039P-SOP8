#include "include.h"
#include "hid_usage.h"
#include "ble_hid_service.h"
#include "app_hid_event.h"
#include "app_hid_selfie.h"
#include "app_hid_tiktok.h"
#include "app_hid_test.h"


#if BLE_HID_TEST_MODE
key_and_hid_message_map_typedef key_and_hid_message_map[] = {
    {KEY_ID_K1 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER},
    {KEY_ID_K1 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_DIGITIZER},
    {KEY_ID_K1 | KEY_THREE,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_MOUSE},
    {KEY_ID_K1 | KEY_FOUR,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_KEYBOARD},
//    {KEY_ID_K1 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER7},
//
//    {KEY_ID_K2 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER1},
//    {KEY_ID_K2 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER2},
//    {KEY_ID_K2 | KEY_THREE,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER3},
//    {KEY_ID_K3 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER4},
//    {KEY_ID_K3 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER5},
//    {KEY_ID_K3 | KEY_THREE,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_TEST_CONSUMER6}
};
#elif (BLE_HID_SELFIE_MODE || BLE_HID_TIKTOK_MODE)
key_and_hid_message_map_typedef key_and_hid_message_map[] = {
#if BLE_HID_SELFIE_MODE
    {KEY_ID_K1 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_PHOTOGRAPH},
    {KEY_ID_K1 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_OPEN_OR_SWITCH_CAMERA},
    {KEY_ID_K1 | KEY_THREE,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_FOCUS_PLATE},
    {KEY_ID_K2 | KEY_SHORT,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_IN},
    {KEY_ID_K2 | KEY_LONG,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_IN_KEEP},
    {KEY_ID_K2 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_IN_KEEP},
    {KEY_ID_K2 | KEY_LONG_UP,       MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_IN_KEEP_STOP},
    {KEY_ID_K3 | KEY_SHORT,         MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_OUT},
    {KEY_ID_K3 | KEY_LONG,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_OUT_KEEP},
    {KEY_ID_K3 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_OUT_KEEP},
    {KEY_ID_K3 | KEY_LONG_UP,       MSG_TYPE_HID_EVENT | BLE_HID_MSG_ZOOM_OUT_KEEP_STOP},
#endif  //BLE_HID_SELFIE_MODE

#if BLE_HID_TIKTOK_MODE
    {KEY_ID_K4 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_CLICK},
    {KEY_ID_K4 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_DOUBLE},
    {KEY_ID_K4 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_K4_HOLD},
    {KEY_ID_K4 | KEY_LONG_UP,       MSG_TYPE_HID_EVENT | BLE_HID_MSG_K4_HOLD_UP},

    {KEY_ID_K5 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_UP},
    {KEY_ID_K5 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_LEFT},
    {KEY_ID_K5 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_K5_HOLD},
    {KEY_ID_K6 | KEY_SHORT_UP,      MSG_TYPE_HID_EVENT | BLE_HID_MSG_DONW},
    {KEY_ID_K6 | KEY_DOUBLE,        MSG_TYPE_HID_EVENT | BLE_HID_MSG_RIGHT},
    {KEY_ID_K6 | KEY_HOLD,          MSG_TYPE_HID_EVENT | BLE_HID_MSG_K6_HOLD},
#endif  //BLE_HID_TIKTOK_MODE
};
#endif // BLE_HID_TEST_MODE


bool ble_hid_post_msg_by_key(u16 key)
{
    u8 table_size = sizeof(key_and_hid_message_map) / sizeof(key_and_hid_message_map_typedef);

    for (u8 i = 0; i < table_size; i++) {
        if (key == key_and_hid_message_map[i].key) {
            msg_enqueue(key_and_hid_message_map[i].msg);
            return true;
        }
    }

    return false;
}

bool ble_hid_send_touch_cmd_android(bool tip_switch, bool in_range, bool touch_valid, u8 contact_id, u16 x, u16 y)
{
    ble_digitizer_report_android_typedef ble_digitizer_report;
    memset(&ble_digitizer_report, 0x00, sizeof(ble_digitizer_report_android_typedef));
    ble_digitizer_report.tip_switch  = tip_switch ;
    ble_digitizer_report.in_range    = in_range   ;
    ble_digitizer_report.touch_valid = touch_valid;
    ble_digitizer_report.contact_id  = contact_id ;
    ble_digitizer_report.xy[0] = x & 0xff;  //android x, y: 0 ~ 1000
    ble_digitizer_report.xy[1] = ((x & 0xf00) >> 8) | ((y & 0x0f) << 4);
    ble_digitizer_report.xy[2] = (y & 0xff0) >> 4;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_DIGITIZER, (u8 *)&ble_digitizer_report, sizeof(ble_digitizer_report_android_typedef));
}

bool ble_hid_send_touch_cmd_ios(u8 tip_switch_l_0, u8 tip_switch_r_0, u8 contact_id_0, u16 x_0, u16 y_0,
                                u8 tip_switch_l_1, u8 tip_switch_r_1, u8 contact_id_1, u16 x_1, u16 y_1,
                                u8 tip_switch_l_2, u8 tip_switch_r_2, u8 contact_id_2, u16 x_2, u16 y_2)
{
    ble_digitizer_report_ios_typedef ble_digitizer_report;
    memset(&ble_digitizer_report, 0x00, sizeof(ble_digitizer_report_ios_typedef));
    ble_digitizer_report.tip_switch_l_0  = tip_switch_l_0;
    ble_digitizer_report.tip_switch_r_0  = tip_switch_r_0;
    ble_digitizer_report.contact_id_0    = contact_id_0  ;
    ble_digitizer_report.xy_0[0] = x_0 & 0xff;  //ios x: 0 ~ 2729; y: 0 ~ 1364
    ble_digitizer_report.xy_0[1] = ((x_0 & 0xf00) >> 8) | ((y_0 & 0x0f) << 4);
    ble_digitizer_report.xy_0[2] = (y_0 & 0xff0) >> 4;

    ble_digitizer_report.tip_switch_l_1 = tip_switch_l_1;
    ble_digitizer_report.tip_switch_r_1 = tip_switch_r_1;
    ble_digitizer_report.contact_id_1   = contact_id_1  ;
    ble_digitizer_report.xy_1[0] = x_1 & 0xff;
    ble_digitizer_report.xy_1[1] = ((x_1 & 0xf00) >> 8) | ((y_1 & 0x0f) << 4);
    ble_digitizer_report.xy_1[2] = (y_1 & 0xff0) >> 4;

    ble_digitizer_report.tip_switch_l_2 = tip_switch_l_2;
    ble_digitizer_report.tip_switch_r_2 = tip_switch_r_2;
    ble_digitizer_report.contact_id_2   = contact_id_2  ;
    ble_digitizer_report.xy_2[0] = x_2 & 0xff;
    ble_digitizer_report.xy_2[1] = ((x_2 & 0xf00) >> 8) | ((y_2 & 0x0f) << 4);
    ble_digitizer_report.xy_2[2] = (y_2 & 0xff0) >> 4;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_DIGITIZER, (u8 *)&ble_digitizer_report, sizeof(ble_digitizer_report_ios_typedef));
}

bool ble_hid_send_mouse_cmd(uint8_t button, int8_t wheel, int8_t x, int8_t y)
{
    ble_mouse_report_typedef ble_mouse_report;
    memset(&ble_mouse_report, 0x00, sizeof(ble_mouse_report_typedef));

    ble_mouse_report.button = button;
    ble_mouse_report.wheel = wheel;
    ble_mouse_report.x = x;
    ble_mouse_report.y = y;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_MOUSE, (u8 *)&ble_mouse_report, sizeof(ble_mouse_report_typedef));
}

bool ble_hid_send_consumer_cmd(u16 cmd)
{
    return ble_hid_task_enqueue(BLE_HID_CMD_ID_CONSUMER, (u8 *)&cmd, 2);
}

bool ble_hid_send_keyboard_cmd(u8 modifier, u8 key_code)
{
    ble_keyboard_report_typedef ble_keyboard_report;
    memset(&ble_keyboard_report, 0x00, sizeof(ble_keyboard_report_typedef));
    ble_keyboard_report.modifier = modifier;
    ble_keyboard_report.key_code = key_code;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_KEYBOARD, (u8 *)&ble_keyboard_report, sizeof(ble_keyboard_report_typedef));
}

AT(.text.func.msg.hid)
bool func_message_hid_event(u16 msg)
{
    bool ret = true;
    switch (msg & BLE_HID_MSG_ID_MASK) {
        case BLE_HID_MSG_ID_SELFIE:
            #if BLE_HID_SELFIE_MODE
                ret = ble_hid_event_selfie(msg);
            #endif  //BLE_HID_SELFIE_MODE
            break;

        case BLE_HID_MSG_ID_TIKTOK:
            #if BLE_HID_TIKTOK_MODE
                ret = ble_hid_event_tiktok(msg);
            #endif  //BLE_HID_TIKTOK_MODE
            break;

        case BLE_HID_MSG_ID_TEST:
            #if BLE_HID_TEST_MODE
                ret = ble_hid_event_test(msg);
            #endif  //BLE_HID_TEST_MODE
            break;

        default:
            ret = false;
            break;
    }

    return ret;
}
