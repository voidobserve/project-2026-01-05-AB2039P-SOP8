#include "include.h"
#include "hid_usage.h"
#include "ble_hid_service.h"
#include "app_hid_event.h"
#include "app_hid_test.h"


#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#define TRACE_R(...)            my_print_r(__VA_ARGS__)
#else // !TRACE_EN
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if BLE_HID_TEST_MODE

bool ble_hid_event_test(u16 msg)
{
    switch (msg & BLE_HID_MSG_MASK) {
        case BLE_HID_MSG_TEST_CONSUMER:
#if LE_HID_CONSUMER
            TRACE("BLE_HID_MSG_TEST_CONSUMER\n");
            /* Switch to Camera. (Only Android Valid.) */
            // ble_hid_send_consumer_cmd(HID_CCC_MEDIA_SELECT_VCR);
            // ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
            // printf("send swith to VCR command\n");

            /*  V+  */
//             ble_hid_send_consumer_cmd(HID_CCC_VOLUME_DECREMENT);
//             ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
//

            /*  >||  */
			ble_hid_send_consumer_cmd(HID_CCC_PLAY_OR_PAUSE);
			ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
			printf("send play/pause command\n");

            /*  |<<  */
            // ble_hid_send_consumer_cmd(HID_CCC_SCAN_NEXT_TRACK);
            // ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
            // printf("send next track command\n");

            /*  >>|  */
            // ble_hid_send_consumer_cmd(HID_CCC_SCAN_PREVIOUS_TRACK);
            // ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
            // printf("send previous track command\n");
#endif  //LE_HID_CONSUMER
            break;
        case BLE_HID_MSG_TEST_DIGITIZER:
#if  LE_HID_DIGITIZER
            TRACE("BLE_HID_MSG_TEST_DIGITIZER\n");
            #if 1
            /* Single finger up */
            ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 800);
            ble_hid_send_touch_cmd_android(1, 1, 1, 0, 500, 200);
            ble_hid_send_touch_cmd_android(0, 0, 0, 0, 500, 200);
            #else
            /* Double finger amplification */
            // Step 1: Press the first point and hold still
            ble_hid_send_touch_cmd_android(1, 0, 500, 800);
            // Step 2: Press another point and slide up some distance
            ble_hid_send_touch_cmd_android(1, 0, 500, 750);
            ble_hid_send_touch_cmd_android(1, 0, 500, 700);
            ble_hid_send_touch_cmd_android(1, 0, 500, 650);
            ble_hid_send_touch_cmd_android(1, 0, 500, 600);
            // Step 3: Release these two points
            ble_hid_send_touch_cmd_android(0, 0, 500, 800);
            ble_hid_send_touch_cmd_android(0, 0, 500, 600);
            #endif
#endif  //LE_HID_DIGITIZER
            break;

        case BLE_HID_MSG_TEST_MOUSE:
#if LE_HID_MOUSE
            TRACE("BLE_HID_MSG_TEST_MOUSE\n");
            //左键单击
            ble_hid_send_mouse_cmd(1, 0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0, 0);

            // //右键单击
            // ble_hid_send_mouse_cmd(2, 0, 0, 0);
            // ble_hid_send_mouse_cmd(0, 0, 0, 0);

            // //中键单击
            // ble_hid_send_mouse_cmd(4, 0, 0, 0);
            // ble_hid_send_mouse_cmd(0, 0, 0, 0);

            // //滚轮向下滚一格
            // ble_hid_send_mouse_cmd(0, -1, 0, 0);

            // //鼠标向右移动一段距离再向左移动一段距离
            // ble_hid_send_mouse_cmd(0, 0, 10, 0);
            // ble_hid_send_mouse_cmd(0, 0, 10, 0);
            // ble_hid_send_mouse_cmd(0, 0, 10, 0);
            // ble_hid_send_mouse_cmd(0, 0, 10, 0);
            // ble_hid_send_mouse_cmd(0, 0, 10, 0);
            // ble_hid_send_mouse_cmd(0, 0, -10, 0);
            // ble_hid_send_mouse_cmd(0, 0, -10, 0);
            // ble_hid_send_mouse_cmd(0, 0, -10, 0);
            // ble_hid_send_mouse_cmd(0, 0, -10, 0);
            // ble_hid_send_mouse_cmd(0, 0, -10, 0);
#endif  //LE_HID_MOUSE
            break;

        case BLE_HID_MSG_TEST_KEYBOARD:
#if LE_HID_KEYBOARD
            TRACE("BLE_HID_MSG_TEST_KEYBOARD\n");
            /* Ctrl + Alt + Del */
            //ble_hid_send_keyboard_cmd(HID_KM_LEFT_CTRL | HID_KM_LEFT_ALT, HID_KB_DELETE_FORWARD);

            ble_hid_send_keyboard_cmd(HID_KM_NONE,0x04);
            ble_hid_send_keyboard_cmd(HID_KM_NONE,0x05);
            ble_hid_send_keyboard_cmd(HID_KM_NONE,0x06);

            ble_hid_send_keyboard_cmd(HID_KM_NONE, HID_KB_RELEASE);
#endif  //LE_HID_KEYBOARD
            break;

        default:
            /* event invalid */
            return false;
    }

    return true;
}

#endif  //BLE_HID_TEST_MODE
