#ifndef __BLE_HID_SERVICE_ATVV_H
#define __BLE_HID_SERVICE_ATVV_H
#include "include.h"

#define BLE_HID_KEY_RELEASE_CMD     0x000000UL
#define BLE_HID_KEY_UP_CMD          0x000042UL
#define BLE_HID_KEY_DOWN_CMD        0x000043UL
#define BLE_HID_KEY_LEFT_CMD        0x000044UL
#define BLE_HID_KEY_RIGHT_CMD       0x000045UL
#define BLE_HID_KEY_MEUN_CMD        0x0001A2UL
#define BLE_HID_KEY_HOME_CMD        0x000223UL
#define BLE_HID_KEY_OK_CMD          0x000041UL
#define BLE_HID_KEY_BACK_CMD        0x000224UL
#define BLE_HID_KEY_VOL_UP_CMD      0x0000E9UL
#define BLE_HID_KEY_VOL_DOWN_CMD    0x0000EAUL
#define BLE_HID_KEY_PWR_CMD         0x000030UL
#define BLE_HID_KEY_VOICE_EN_CMD    0x0000CFUL
#define BLE_HID_KEY_VOICE_DIS_CMD   0           //无主动关闭命令
#define BLE_HID_KEY_NONE_CMD        0xFFFFFFUL


#define BLE_HID_CMD_BUF_LEN            4
#define BLE_HID_CMD_BUF_MASK           (BLE_HID_CMD_BUF_LEN - 1)
#define BLE_HID_RX_BUF_LEN             20

#define BLE_HID_NOTIFY_DATA_BUF_LEN    3

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_HID_RX_BUF_LEN];
    uint16_t handle;
}ble_hid_rx_cmd_t;

typedef struct{
    ble_hid_rx_cmd_t cmd[BLE_HID_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}ble_hid_rx_cb_t;

//TX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_HID_NOTIFY_DATA_BUF_LEN];
    uint16_t handle;
}ble_hid_tx_t;

typedef struct{
    uint16_t adv_interval;
    uint16_t conn_interval;
    uint16_t conn_latency;
    uint16_t conn_timeout;
}ble_hid_var_t;

void ble_hid_service_notify_for_handle(uint16_t att_handle, uint32_t cmd);
void ble_hid_service_init(void);
void ble_hid_service_proc(void);

#endif
