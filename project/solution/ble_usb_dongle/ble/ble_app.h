/**
******************************************************************************************************************************
*
*@file ble_app.h
*
*@brief Head file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef __BLE_APP_H
#define __BLE_APP_H

#define MAX_BLE_CONNECTIONS                 1

#define INVALID_CONIDX                  (0xff)
#define INVALID_HANDLE                  (0xff)

typedef struct _ble_cb_t {
    ble_con_handle_t conhdl;
} ble_cb_t;


void ble_info_init(void);
ble_cb_t *ble_get_ble_info(void);
uint8_t ble_get_free_conidx(void);
uint8_t ble_get_conidx_for_handle(uint16_t conhdl);
uint8_t ble_get_conn_cnt(void);
void ble_advertising_report_callback(uint8_t evt_type, uint8_t addr_type, uint8_t *addr, uint8_t adv_data_len, const uint8_t *adv_data, char rssi);

#endif
