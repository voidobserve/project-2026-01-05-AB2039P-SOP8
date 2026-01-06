#ifndef __BLE_PROFILE_H
#define __BLE_PROFILE_H

//
// list service handle ranges
//
#define ATT_SERVICE_FF20_START_HANDLE 0x0001
#define ATT_SERVICE_FF20_END_HANDLE 0x0006

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_FF21_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_FF22_01_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_FF22_01_CLIENT_CONFIGURATION_HANDLE 0x0006

const u8 *ble_get_profile_data(void);

#endif
