#include "include.h"
#include "ble_hid_service.h"
#include "app_hid_event.h"
#include "app_hid_tiktok.h"
#include "hid_usage.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if BLE_HID_TIKTOK_MODE

#define LIKE_DELAY_CNT_MAX      3   //连续点赞间隔
static uint8_t like_delay_cnt = 0;

bool ble_hid_event_tiktok(u16 msg)
{
    uint8_t is_android = (PEER_DEVICE_TYPE_ANDROID == ble_hid_peer_device_type_get());

    switch (msg & BLE_HID_MSG_MASK) {
        // 单击
        case BLE_HID_MSG_CLICK:
            TRACE("BLE_HID_MSG_CLICK\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 400);  //android x, y: 0 ~ 1000
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);  //ios x: 0 ~ 2729; y: 0 ~ 1364
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        // 双击
        case BLE_HID_MSG_DOUBLE:
            TRACE("BLE_HID_MSG_DOUBLE\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 400);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);

                //等待
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);

                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 400);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);

                //等待
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);

                ble_hid_send_touch_cmd_ios(1, 1, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        // 连点
        case BLE_HID_MSG_K4_HOLD:
            TRACE("BLE_HID_MSG_K4_HOLD\n");
            like_delay_cnt++;
            if (like_delay_cnt < LIKE_DELAY_CNT_MAX) {
                break;
            } else {
                like_delay_cnt = 0;
            }

            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 400);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 500, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_K4_HOLD_UP:
            TRACE("BLE_HID_MSG_K4_HOLD_UP\n");
            like_delay_cnt = 0;
            break;

        case BLE_HID_MSG_K5_HOLD:
            TRACE("BLE_HID_MSG_K5_HOLD\n");
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VCR_PLUS);
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            break;

        case BLE_HID_MSG_HOME:
            TRACE("BLE_HID_MSG_HOME\n");
            if (is_android) {
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_AC_HOME);
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            } else {
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_MENU);
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            }
            break;

        case BLE_HID_MSG_VOLUME_UP:
            TRACE("BLE_HID_MSG_VOLUME_UP\n");
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_INC);
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            break;

        case BLE_HID_MSG_VOLUME_DOWN:
            TRACE("BLE_HID_MSG_VOLUME_DOWN\n");
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_DEC);
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            break;

        case BLE_HID_MSG_PHOTOGRAPH:
            TRACE("BLE_HID_MSG_PHOTOGRAPH\n");
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_INC);
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            break;

        case BLE_HID_MSG_OPEN_OR_SWITCH_CAMERA:
            TRACE("BLE_HID_MSG_OPEN_OR_SWITCH_CAMERA\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 300, 450);   //android x, y: 0 ~ 1000
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 300, 450);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 300, 500);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 300, 550);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 300, 500);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 300, 500);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 850, 850);
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_MODE_STEP);
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 850, 850);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 2456, 1266,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);  //ios x: 0 ~ 2729; y: 0 ~ 1364
                ble_hid_send_touch_cmd_ios(0, 0, 0, 2456, 1266,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 2456, 1202,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 2456, 1202,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        //上一个视频
        case BLE_HID_MSG_UP:
            TRACE("BLE_HID_MSG_UP\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 350);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 350);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 380);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 410);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 440);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 470);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 530);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 1200);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 530);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 300,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 400,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 600,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 700,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 800,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 900,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 600, 900,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        //下一个视频
        case BLE_HID_MSG_DONW:
            TRACE("BLE_HID_MSG_DONW\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 620);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 590);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 560);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 530);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 500);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 470);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 0);
                ble_hid_send_touch_cmd_android(0, 1, 0, 0, 500, 470);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 470);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 800,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 700,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 600,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 500,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 400,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 300,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 200,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 600, 100,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

            break;

        case BLE_HID_MSG_LEFT:
            TRACE("BLE_HID_MSG_LEFT\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 300, 250);
                ble_hid_send_touch_cmd_android(0, 1, 0, 0, 300, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 300, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 340, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 380, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 420, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 700, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 950, 255);
                ble_hid_send_touch_cmd_android(0, 1, 0, 0, 950, 250);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 950, 250);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 300, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 900, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1200, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1500, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1800, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 2100, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 2100, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_RIGHT:
            TRACE("BLE_HID_MSG_RIGHT\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 700, 250);
                ble_hid_send_touch_cmd_android(0, 1, 0, 0, 700, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 700, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 660, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 620, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 580, 250);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 300, 255);
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 50,  255);
                ble_hid_send_touch_cmd_android(0, 1, 0, 0, 50,  250);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 50,  250);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 2100, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1800, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1500, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1200, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 900, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 600, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 300, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 300, 360,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
            break;

        default:
            /* event invalid */
            return false;
    }

    return true;
}

#endif  //BLE_HID_TIKTOK_MODE
