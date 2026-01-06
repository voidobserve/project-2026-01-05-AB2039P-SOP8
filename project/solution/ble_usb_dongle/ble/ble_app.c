/**
******************************************************************************************************************************
*
*@file ble_app.c
*
*@brief Source file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "include.h"
#include "ble_app.h"
#include "ble_init.h"

ble_cb_t ble_cb[MAX_BLE_CONNECTIONS] AT(.ble_buf.var);

void ble_info_init(void)
{
    memset(&ble_cb, 0, sizeof(ble_cb));

    for (uint8_t i = 0; i < MAX_BLE_CONNECTIONS; i ++) {
        ble_cb[i].conhdl = INVALID_HANDLE;
    }
}

ble_cb_t *ble_get_ble_info(void)
{
    return ble_cb;
}

uint8_t ble_get_free_conidx(void)
{
    uint8_t conidx;

    // Find first available connection index
    for (conidx = 0; conidx < MAX_BLE_CONNECTIONS; conidx ++) {
        // Find first index which is free.
        if (ble_cb[conidx].conhdl == INVALID_HANDLE) {
            return conidx;
        }
    }

    return INVALID_CONIDX;
}

uint8_t ble_get_conidx_for_handle(uint16_t conhdl)
{
    uint8_t conidx;

    // Find first available connection index
    for (conidx = 0; conidx < MAX_BLE_CONNECTIONS; conidx ++) {
        // Find first index which is free.
        if (ble_cb[conidx].conhdl == conhdl) {
            return conidx;
        }
    }

    return INVALID_CONIDX;
}


uint8_t ble_get_conn_cnt(void)
{
    uint8_t conn_cnt = 0;

    uint8_t loop_cnt = MAX_BLE_CONNECTIONS - 1;

    do {
        if (ble_cb[loop_cnt].conhdl != INVALID_HANDLE) {
            conn_cnt ++;
        }
    }while (loop_cnt --);

    return conn_cnt;
}


void ble_advertising_report_callback(uint8_t evt_type, uint8_t addr_type, uint8_t *addr, uint8_t adv_data_len, const uint8_t *adv_data, char rssi)
{
    //printf("evt_type %x, addr_type %x, Adv Addr:", evt_type, addr_type);
    //print_r(addr, 6);
    //printf("Adv Data: %d###", adv_data_len);
    //print_r(adv_data, adv_data_len);

    uint8_t con_dev_name1[] = "LE_AB202X";
    uint8_t find1 = 0;

    if (adv_data[1] == 0x09) {
        if (!strncmp((char *)&adv_data[2], (const char *)con_dev_name1, adv_data_len - 2)) {
            find1 = 1;
            printf ("find device %s\n", con_dev_name1);
        }
    }

    if(find1){
        if (ble_get_conn_cnt() < MAX_BLE_CONNECTIONS) {
            ble_scan_dis();

            printf("ble_connect_by_addr: ");
            print_r(addr, 6);

            ble_connect_by_addr(addr, addr_type, 24, 0, 400);
        }else {
            printf("Master device(%d) reaches the upper limit(%d)\n", ble_get_conn_cnt(), MAX_BLE_CONNECTIONS);
        }
    }
}

