#ifndef _APP_HID_EVENT_H
#define _APP_HID_EVENT_H

#include "ble_hid_service.h"

#define BLE_HID_MSG_ID_SELFIE           0x0000
#define BLE_HID_MSG_ID_TIKTOK           0x0100
#define BLE_HID_MSG_ID_TEST             0x0200
#define BLE_HID_MSG_ID_MASK             0x0f00

typedef enum {
    BLE_HID_MSG_NONE,

    /* Selfie Device Message */
    BLE_HID_MSG_PHOTOGRAPH              = 0x0001,
    BLE_HID_MSG_OPEN_OR_SWITCH_CAMERA,
    BLE_HID_MSG_VCR_START,
    BLE_HID_MSG_VCR_STOP,
    BLE_HID_MSG_ZOOM_IN,
    BLE_HID_MSG_ZOOM_IN_KEEP,
    BLE_HID_MSG_ZOOM_IN_KEEP_STOP,
    BLE_HID_MSG_ZOOM_OUT,
    BLE_HID_MSG_ZOOM_OUT_KEEP,
    BLE_HID_MSG_ZOOM_OUT_KEEP_STOP,
    /* Only IOS Used */
    BLE_HID_MSG_FOCUS_PLATE,

    /* TikTok Page turner Message */
    BLE_HID_MSG_CLICK                   = 0x0101,
    BLE_HID_MSG_DOUBLE,
    BLE_HID_MSG_UP,             //上一个视频          
    BLE_HID_MSG_DONW,           //下一个视频
    BLE_HID_MSG_LEFT,
    BLE_HID_MSG_RIGHT,
    BLE_HID_MSG_ROUGH_MOVE,
    BLE_HID_MSG_CAREFUL_MOVE,
    BLE_HID_MSG_VOLUME_UP,
    BLE_HID_MSG_VOLUME_DOWN,
    BLE_HID_MSG_HOME,
    BLE_HID_MSG_PP_HOLD,
    BLE_HID_MSG_K4_HOLD,
    BLE_HID_MSG_K4_HOLD_UP,
    BLE_HID_MSG_K5_HOLD,
    BLE_HID_MSG_K6_HOLD,


    /* Test Message */
    BLE_HID_MSG_TEST_CONSUMER           = 0x0201,
    BLE_HID_MSG_TEST_CONSUMER1,
    BLE_HID_MSG_TEST_CONSUMER2,
    BLE_HID_MSG_TEST_CONSUMER3,
    BLE_HID_MSG_TEST_CONSUMER4,
    BLE_HID_MSG_TEST_CONSUMER5,
    BLE_HID_MSG_TEST_CONSUMER6,
    BLE_HID_MSG_TEST_CONSUMER7,
    BLE_HID_MSG_TEST_DIGITIZER,
    BLE_HID_MSG_TEST_MOUSE,
    BLE_HID_MSG_TEST_KEYBOARD,

    BLE_HID_MSG_MASK                    = 0x0fff,
} BLE_HID_MSG_TYPEDEF;


typedef struct {
    uint16_t        key;
    uint16_t        msg;
} key_and_hid_message_map_typedef;


void app_hid_test_func(void);
bool ble_hid_post_msg_by_key(u16 key);
bool func_message_hid_event(u16 msg);
bool ble_hid_send_touch_cmd_android(bool tip_switch, bool in_range, bool touch_valid, u8 contact_id, u16 x, u16 y);
bool ble_hid_send_touch_cmd_ios(u8 tip_switch_l_0, u8 tip_switch_r_0, u8 contact_id_0, u16 x_0, u16 y_0, 
                                u8 tip_switch_l_1, u8 tip_switch_r_1, u8 contact_id_1, u16 x_1, u16 y_1, 
                                u8 tip_switch_l_2, u8 tip_switch_r_2, u8 contact_id_2, u16 x_2, u16 y_2);

bool ble_hid_send_mouse_cmd(uint8_t button, int8_t wheel, int8_t x, int8_t y);
bool ble_hid_send_consumer_cmd(u16 cmd);
bool ble_hid_send_keyboard_cmd(u8 modifier, u8 key_code);

#endif // _APP_HID_EVENT_H
