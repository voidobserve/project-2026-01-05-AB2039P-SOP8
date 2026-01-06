#ifndef _BLE_HID_SERVICE_H
#define _BLE_HID_SERVICE_H

#include "macro.h"

#define BLE_HID_CMD_BUFFER_LENGTH           30
#define BLE_HID_CMD_QUEUE_SIZE              30
#define BLE_HID_POLLING_INTERVAL_MS         30

/**
 * @brief The keyboard modifier definition.
 */
#define HID_KM_NONE                         0x00
#define HID_KM_LEFT_CTRL                    0x01
#define HID_KM_LEFT_SHIFT                   0x02
#define HID_KM_LEFT_ALT                     0x04
#define HID_KM_LEFT_GUI                     0x08
#define HID_KM_RIGHT_CTRL                   0x01
#define HID_KM_RIGHT_SHIFT                  0x02
#define HID_KM_RIGHT_ALT                    0x04
#define HID_KM_RIGHT_GUI                    0x08

typedef enum {
    BLE_HID_CMD_ID_CONSUMER,
    BLE_HID_CMD_ID_DIGITIZER,
    BLE_HID_CMD_ID_MOUSE,
    BLE_HID_CMD_ID_KEYBOARD,
} BLE_HID_CMD_ID_TYPEDEF;


typedef struct PACKED {
    uint8_t         tip_switch  : 1;
    uint8_t         in_range    : 1;
    uint8_t         touch_valid : 1;
    uint8_t         contact_id  : 5;
    uint8_t         xy[3];   // X low 12 bit, y high 12 bit.  x, y: 0 ~ 1000.
} ble_digitizer_report_android_typedef;

typedef struct PACKED {
    uint8_t         tip_switch_l_0  : 1;
    uint8_t         tip_switch_r_0  : 1;
    uint8_t         contact_id_0    : 6;
    uint8_t         xy_0[3];   // X low 12 bit, y high 12 bit.  x: 0 ~ 2729; y: 0 ~ 1364.
    uint8_t         tip_switch_l_1  : 1;
    uint8_t         tip_switch_r_1  : 1;
    uint8_t         contact_id_1    : 6;
    uint8_t         xy_1[3];
    uint8_t         tip_switch_l_2  : 1;
    uint8_t         tip_switch_r_2  : 1;
    uint8_t         contact_id_2    : 6;
    uint8_t         xy_2[3];
    const uint8_t   reserve;
} ble_digitizer_report_ios_typedef;

typedef struct PACKED {
    union {
        struct {
            uint8_t btn_left    : 1;
            uint8_t btn_right   : 1;
            uint8_t btn_central : 1;
            uint8_t btn_4       : 1;
            uint8_t btn_5       : 1;
            uint8_t reserved    : 3;
        };
        uint8_t button;
    };
    int8_t  wheel;
    int8_t  x;
    int8_t  y;
} ble_mouse_report_typedef;

typedef struct {
    uint8_t modifier;
    uint8_t key_code;
} ble_keyboard_report_typedef;

typedef struct {
    BLE_HID_CMD_ID_TYPEDEF  id;
    uint8_t                 len;
    uint8_t                 buffer[BLE_HID_CMD_BUFFER_LENGTH];
} ble_hid_cmd_typedef;

typedef struct {
    uint8_t                 head_idx;
    uint8_t                 rear_idx;
    ble_hid_cmd_typedef     queue[BLE_HID_CMD_QUEUE_SIZE];
} ble_hid_tasks_typedef;


void ble_hid_service_init(void);
void ble_hid_service_proc(void);

/* Transpor Layer API */
bool ble_hid_task_enqueue(BLE_HID_CMD_ID_TYPEDEF id, u8 *buffer, u8 len);
int ble_hid_report_for_handle(u16 handle, u8 *buffer, u8 len);
int ble_hid_report_for_handle_kick(u16 handle, u8 *buffer, u8 len, uint8_t kick_cfg);

#endif // _BLE_HIDS_SERVICE_H
