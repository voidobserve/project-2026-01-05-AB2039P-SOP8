#include "include.h"

AT(.rodata.adv)
const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
#if LE_HID_EN
	// Apperance
    0x03, 0x19, 0xc1, 0x03,
    // Human Interface Device
    0x03, 0x03, 0x12, 0x18,
#endif // LE_HID_EN
};

AT(.rodata.scan)
const uint8_t scan_data_const[] = {
};

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    //读取BLE配置的蓝牙名称
    int name_len;
    name_len = strlen(xcfg_cb.le_name);
    if ((data_len + name_len + 2) > 31) {
        printf("[err] scan response buffer insufficient!!!\n");
        return data_len;
    }
    scan_buf[data_len+1] = 0x09; //ad type local name
    if (name_len > 0) {
        memcpy(&scan_buf[data_len+2], xcfg_cb.le_name, name_len);
        scan_buf[data_len] = name_len + 1;  //ad length
        data_len = data_len + 2 + name_len;
    }

    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    //读取BLE配置的蓝牙名称
    int name_len;
    name_len = strlen(xcfg_cb.le_name);
    if ((data_len + name_len + 2) > 31) {
        printf("[err] adv buffer space insufficient!!!\n");
        return data_len;
    }
    adv_buf[data_len+1] = 0x09; //ad type local name
    if (name_len > 0) {
        memcpy(&adv_buf[data_len+2], xcfg_cb.le_name, name_len);
        adv_buf[data_len] = name_len + 1;  //ad length
        data_len = data_len + 2 + name_len;
    }

    return data_len;
}

void ble_adv_param_init(void)
{
    adv_param_t adv_info;

    memset(&adv_info,0,sizeof(adv_info));
    adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data));
    adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
    adv_info.adv_int_min = adv_info.adv_int_max = 48;  //30ms
    adv_info.adv_type = ADV_TYPE_IND;
    adv_info.adv_en = 1;
    adv_info.channel_map = ADV_ALL_CHNLS_EN;

    ble_set_adv_param(&adv_info);
}
