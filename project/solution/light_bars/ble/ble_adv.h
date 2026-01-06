#ifndef __BLE_ADV_H
#define __BLE_ADV_H

void ble_adv_param_init(void);
u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size);
u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size);

#endif
