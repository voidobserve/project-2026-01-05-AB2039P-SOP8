#ifndef __BLE_PROC_H
#define __BLE_PROC_H

void ble_app_proc(void);
bool ble_proc_pending(void);
void ble_enter_sleep_proc(void);
void ble_exit_sleep_proc(void);
bool ble_proc_pending(void);
void ble_user_set_service_pending(void);

#endif
