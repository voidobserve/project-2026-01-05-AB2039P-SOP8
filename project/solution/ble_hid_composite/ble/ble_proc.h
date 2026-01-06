#ifndef __BLE_PROC_H
#define __BLE_PROC_H


typedef enum {
    PEER_DEVICE_TYPE_ANDROID            = 0x01,
    PEER_DEVICE_TYPE_IOS,
} PEER_DEVICE_TYPE_TYPEDEF;


void ble_app_proc(void);
bool ble_proc_pending(void);
void ble_enter_sleep_proc(void);
void ble_exit_sleep_proc(void);
void ble_hid_peer_device_type_set(PEER_DEVICE_TYPE_TYPEDEF type);
PEER_DEVICE_TYPE_TYPEDEF ble_hid_peer_device_type_get(void);
bool ble_hid_peer_device_is_ios(void);
void ble_set_service_pending(bool state);
bool ble_get_service_pending(void);

#endif
