#ifndef __BLE_CLIENT_H
#define __BLE_CLIENT_H
#include "include.h"

#if VC_RM_DG_ADPCM_DEC_EN
#define MAX_SERVICE_NUM              10
#define MAX_CHARACTERISTIC_NUM       36

#elif VC_RM_DG_mSBC_DEC_EN
#define MAX_SERVICE_NUM              10
#define MAX_CHARACTERISTIC_NUM       62

#elif VC_RM_DG_SBC_DEC_EN
#define MAX_SERVICE_NUM              10
#define MAX_CHARACTERISTIC_NUM       62

#else
#define MAX_SERVICE_NUM              5
#define MAX_CHARACTERISTIC_NUM       20
#endif

#define BLE_CON_SLEEP_INTERVAL    400   //500ms
#define BLE_CON_SLEEP_LATENCY     0
#define BLE_CON_SLEEP_TIMEOUT     1000   //10s


typedef struct
{
    gatt_client_service_t  service[MAX_SERVICE_NUM];
    gatt_client_characteristic_t characteristic[MAX_CHARACTERISTIC_NUM];
    uint8_t service_cnt;
    uint8_t characteristic_cnt;
    uint8_t service_index;
    uint8_t characteristic_index;
    uint16_t conn_handle;
} server_info_t;


void ble_client_init(void);
void ble_app_proc(void);
void ble_enter_sleep_proc(void);
void ble_exit_sleep_proc(void);
server_info_t *ble_get_server_info(uint8_t conidx);

#endif
