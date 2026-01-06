#ifndef __BLE_USER_SERVICE_H
#define __BLE_USER_SERVICE_H


#define BLE_DEFAULT_DEVICE_NAME     "LE_AB202x"

void ble_user_service_init(void);
void ble_user_service_proc(void);
void ble_user_service_changed_indicate(void);
#endif
