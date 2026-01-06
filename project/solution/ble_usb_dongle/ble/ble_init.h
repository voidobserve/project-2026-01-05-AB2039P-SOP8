#ifndef __BLE_INIT_H
#define __BLE_INIT_H

#include "ble_alarm.h"
#include "ble_profile.h"

void bsp_ble_init(void);
void ble_set_work_mode(BLE_WORK_MODE mode);
BLE_WORK_MODE ble_get_work_mode(void);

#endif
