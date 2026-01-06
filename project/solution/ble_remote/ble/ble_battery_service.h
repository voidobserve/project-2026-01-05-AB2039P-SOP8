#ifndef __BLE_BATTERY_SERVICE_ATVV_H
#define __BLE_BATTERY_SERVICE_ATVV_H
#include "include.h"

#define BLE_BAT_CMD_BUF_LEN            4
#define BLE_BAT_CMD_BUF_MASK           (BLE_BAT_CMD_BUF_LEN - 1)
#define BLE_BAT_RX_BUF_LEN             20

#define BLE_BAT_NOTIFY_DATA_BUF_LEN    8

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_BAT_RX_BUF_LEN];
    uint16_t handle;
}ble_bat_rx_cmd_t;

typedef struct{
    ble_bat_rx_cmd_t cmd[BLE_BAT_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}ble_bat_rx_cb_t;

//TX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_BAT_NOTIFY_DATA_BUF_LEN];
    uint16_t handle;
}ble_bat_tx_t;

typedef struct{
}ble_bat_var_t;

void ble_battery_service_init(void);
void ble_battery_service_proc(void);

#endif
