#ifndef _BLE_HIDS_SERVICE_H
#define _BLE_HIDS_SERVICE_H


// report 1 command
#define HID_LEFT_CTRL                       0x01
#define HID_LEFT_SHIFT                      0x02
#define HID_LEFT_ALT                        0x04
#define HID_LEFT_GUI                        0x08
#define HID_RIGHT_CTRL                      0x10
#define HID_RIGHT_SHIFT                     0x20
#define HID_RIGHT_ALT                       0x40
#define HID_RIGHT_GUI                       0x00
#define HID_KEY_E                           0x08        // Goodnote等软件可以按 “E” 切换橡皮擦
#define HID_KEY_P                           0x13        // Goodnote等软件可以按 “P” 切换笔
#define HID_KEY_Z                           0x1d

// report 2 command
#define HID_CONSUMER_MENU                   0x0040
#define HID_CONSUMER_SEARCH                 0x0221
#define HID_CONSUMER_VOL_INC                0x00e9
#define HID_CONSUMER_VOL_DEC                0x00ea
#define HID_CONSUMER_POWER                  0x0030

// system key message
#define MSG_SEND_GO_HOME_CMD                0x0001
#define MSG_SEND_MUTITASK_BAR_CMD           0x0002
#define MSG_SEND_SEARCH_BAR                 0x0004
#define MSG_SEND_VOL_INC_CMD                0x0008
#define MSG_SEND_VOL_DEC_CMD                0x0010
#define MSG_SEND_LOCK_WAKE_SCREEN_CMD       0x0020
#define MSG_SEND_UNDO_CMD                   0x0040
#define MSG_SEND_ERASER_SWITCH_CMD          0x0080
#define MSG_SEND_PEN_SWITCH_CMD             0x0100

extern volatile u16 key_event;

void ble_hids_service_init(void);
void ble_hids_service_proc(void);

#endif // _BLE_HIDS_SERVICE_H