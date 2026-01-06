#ifndef __BLE_INIT_H
#define __BLE_INIT_H

#include "ble_service.h"
#include "ble_alarm.h"
#include "ble_profile.h"
#include "ble_adv.h"
#include "api_btstack.h"

typedef struct _ble_cb_t {
    bool pending;
    ble_con_handle_t con_handle;
} ble_cb_t;

extern ble_cb_t ble_cb;

void bsp_ble_init(void);
void ble_set_work_mode(BLE_WORK_MODE mode);
BLE_WORK_MODE ble_get_work_mode(void);
bool ble_get_bond_info(uint8_t index, sm_bonding_info_t* bond_info);
void ble_dir_adv_param_init(uint8_t index);
void ble_adv_param_init(LE_REMOTE_ADV_PKT adv_pkt);
void ble_rpa_param_init(void);
void ble_rpa_add_dev_to_list(uint8_t index);

#endif
