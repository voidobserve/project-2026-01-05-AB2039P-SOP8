#include "include.h"
#include "hid_usage.h"
#include "ble_hid_service.h"
#include "app_hid_event.h"
#include "app_hid_selfie.h"


#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#define TRACE_R(...)            my_print_r(__VA_ARGS__)
#else // !TRACE_EN
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if BLE_HID_SELFIE_MODE

#define ANDROID_ZOOM_DELAY_CNT_MAX    4
#define IOS_ZOOM_DELAY_CNT_MAX        2
static uint8_t zoom_delay_cnt = 0;

bool ble_hid_event_selfie(u16 msg)
{
    static bool photograph_flag = 0;
    uint8_t is_android = (PEER_DEVICE_TYPE_ANDROID == ble_hid_peer_device_type_get());
    uint8_t zoom_delay_cnt_max = is_android ? ANDROID_ZOOM_DELAY_CNT_MAX : IOS_ZOOM_DELAY_CNT_MAX;

    switch (msg & BLE_HID_MSG_MASK) {
        case BLE_HID_MSG_PHOTOGRAPH:
            TRACE("BLE_HID_MSG_PHOTOGRAPH\n");
            if (photograph_flag) {
                photograph_flag = 0;
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_INC);
            } else {
                photograph_flag = 1;
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_DEC);
            }
            ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
            break;

        case BLE_HID_MSG_VCR_START:
            TRACE("BLE_HID_MSG_VCR_START\n");
                // ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VCR_PLUS);
                // ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
                ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_VOL_INC);
            break;

        case BLE_HID_MSG_VCR_STOP:
            TRACE("BLE_HID_MSG_VCR_STOP\n");
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

        case BLE_HID_MSG_ZOOM_IN:
            TRACE("BLE_HID_MSG_ZOOM_IN\n");
            if (is_android) {
                #if LE_HID_KEYBOARD
                    //放大一个单位，部分手机不支持该指令
                    ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_AC_ZOOM_IN);
                    ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
                #elif LE_HID_DIGITIZER
                    //双指放大一次
                    ble_hid_send_touch_cmd_android(1, 1, 1, 0, 340, 310);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 1, 650, 500);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 650, 500);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 670, 510);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 690, 520);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 710, 530);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 730, 540);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 0, 340, 310);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 1, 730, 540);
                #endif
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 635, 635,
                                           1, 1, 1, 785, 785,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 630, 630,
                                           0, 0, 1, 790, 790,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_ZOOM_IN_KEEP:
            TRACE("BLE_HID_MSG_ZOOM_IN_KEEP\n");
            zoom_delay_cnt++;
            if (zoom_delay_cnt < zoom_delay_cnt_max) {
                break;
            } else {
                zoom_delay_cnt = 0;
            }

            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 340, 310);
                ble_hid_send_touch_cmd_android(0, 0, 0, 1, 650, 500);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 650, 500);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 670, 510);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 690, 520);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 710, 530);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 730, 540);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 340, 310);
                ble_hid_send_touch_cmd_android(0, 0, 0, 1, 730, 540);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 635, 635,
                                           1, 1, 1, 785, 785,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 630, 630,
                                           0, 0, 1, 790, 790,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_ZOOM_IN_KEEP_STOP:
            TRACE("BLE_HID_MSG_ZOOM_IN_KEEP_STOP\n");
            zoom_delay_cnt = 0;
            break;

        case BLE_HID_MSG_ZOOM_OUT:
            TRACE("BLE_HID_MSG_ZOOM_OUT\n");
            if (is_android) {
                #if LE_HID_KEYBOARD
                    //缩小一个单位，部分手机不支持该指令
                    ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_AC_ZOOM_OUT);
                    ble_hid_send_consumer_cmd(BLE_HID_CS_CMD_RELEASE);
                #elif LE_HID_DIGITIZER
                    //双指放大一次
                    ble_hid_send_touch_cmd_android(1, 1, 1, 0, 340, 310);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 1, 730, 540);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 730, 540);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 710, 530);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 690, 520);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 670, 510);
                    ble_hid_send_touch_cmd_android(1, 1, 1, 1, 650, 500);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 0, 340, 310);
                    ble_hid_send_touch_cmd_android(0, 0, 0, 1, 650, 500);
                #endif  //LE_HID_KEYBOARD
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 635, 635,
                                           1, 1, 1, 785, 785,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 640, 640,
                                           0, 0, 1, 780, 780,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_ZOOM_OUT_KEEP:
            TRACE("BLE_HID_MSG_ZOOM_OUT_KEEP\n");
            zoom_delay_cnt++;
            if (zoom_delay_cnt < zoom_delay_cnt_max) {
                break;
            } else {
                zoom_delay_cnt = 0;
            }
            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 340, 310);
                ble_hid_send_touch_cmd_android(0, 0, 0, 1, 730, 540);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 730, 540);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 710, 530);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 690, 520);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 670, 510);
                ble_hid_send_touch_cmd_android(1, 1, 1, 1, 650, 500);
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 340, 310);
                ble_hid_send_touch_cmd_android(0, 0, 0, 1, 650, 500);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 630, 630,
                                           1, 1, 1, 790, 790,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 635, 635,
                                           1, 1, 1, 785, 785,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(1, 1, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 640, 640,
                                           1, 1, 1, 780, 780,
                                           0, 0, 2, 0, 0);
                ble_hid_send_touch_cmd_ios(0, 0, 0, 640, 640,
                                           0, 0, 1, 780, 780,
                                           0, 0, 2, 0, 0);
            }
            break;

        case BLE_HID_MSG_ZOOM_OUT_KEEP_STOP:
            TRACE("BLE_HID_MSG_ZOOM_OUT_KEEP_STOP\n");
            zoom_delay_cnt = 0;
            break;

        case BLE_HID_MSG_FOCUS_PLATE:
            TRACE("BLE_HID_MSG_FOCUS_PLATE\n");
            if (is_android) {
                ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 400);    //android x, y: 0 ~ 1000
                ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 400);
            } else {
                ble_hid_send_touch_cmd_ios(1, 1, 0, 1364, 528,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);  //ios x: 0 ~ 2729; y: 0 ~ 1364
                ble_hid_send_touch_cmd_ios(0, 0, 0, 1364, 528,
                                           0, 0, 1, 0, 0,
                                           0, 0, 2, 0, 0);
            }
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

        default:
            /* event invalid */
            return false;
    }

    return true;
}

#endif  //BLE_HID_SELFIE_MODE
