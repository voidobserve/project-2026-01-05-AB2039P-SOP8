#include "ble_init.h"
#include "ble_adv.h"
#include "ble_service.h"
#include "ble_proc.h"

#define ADV_DEBUG_EN 1

#if ADV_DEBUG_EN
#define ADV_DEBUG(...)                  printf(__VA_ARGS__)
#define ADV_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define ADV_DEBUG(...)
#define ADV_DEBUG_R(...)
#endif

//配对广播
const uint8_t adv_data_const_pair[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x05,
    // Manufacturer Specific Data
    0x03, 0xFF, 0x00, 0x00,
    // Short Local Name
    0x06, 0x08, 0x4D, 0x49, 0x20, 0x52, 0x43,
    // Service Uuids
    0x03, 0x02, 0x12, 0x18,
    // SSP OOB Class of Device
    0x04, 0x0D, 0x04, 0x05, 0x00,
    // Tx Power Level
    0x02, 0x0A, 0x00,
};

//开机广播
uint8_t adv_data_poweron[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x04,
    // Manufacturer Specific Data
    0x03, 0xFF, 0x00, 0x01,
    // Short Local Name
    0x06, 0x08, 0x4D, 0x49, 0x20, 0x52, 0x43,
    // Service Uuids
    0x03, 0x02, 0x12, 0x18,
    // SSP OOB Class of Device
    0x04, 0x0D, 0x04, 0x05, 0x00,
    // Tx Power Level
    0x02, 0x0A, 0x00,
    //私有协议
    0x04, 0xFE, 0x6D, 0x4B, 0x5A,
};

//关机后广播
uint8_t adv_data_const_poweroff[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x04,
    // Manufacturer Specific Data
    0x03, 0xFF, 0x00, 0x00,
    // Short Local Name
    0x06, 0x08, 0x4D, 0x49, 0x20, 0x52, 0x43,
    // Service Uuids
    0x03, 0x02, 0x12, 0x18,
    // SSP OOB Class of Device
    0x04, 0x0D, 0x04, 0x05, 0x00,
    // Tx Power Level
    0x02, 0x0A, 0x00,
};

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

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size, LE_REMOTE_ADV_PKT adv_pkt)
{
    u32 data_len = 0;
    sm_bonding_info_t bond_info;
    memset(adv_buf, 0, buf_size);
    bool have_bd = ble_get_bond_info(0, &bond_info);  //bonding info index 0

    ADV_DEBUG("%s, adv_pkt:%d, have_bd:%d\n", __func__, adv_pkt, have_bd);

    if ((adv_pkt == LE_REMOTE_PKT_DEFAULT) && !have_bd) {
        //上电无配对记录，自动开配对广播
        //上电有配对记录，开了定向广播则自动发定向广播（回连）；未开定向广播则不广播任何数据但是scan rsp包会返回蓝牙名，也会自动回连.
        data_len = sizeof(adv_data_const_pair);
        memcpy(adv_buf, adv_data_const_pair, sizeof(adv_data_const_pair));

    } else if ((adv_pkt == LE_REMOTE_PKT_POWERON) && have_bd) {
        //按键开机发开机广播
        data_len = sizeof(adv_data_poweron);
        adv_data_poweron[28] = bond_info.address.addr[5];
        adv_data_poweron[29] = bond_info.address.addr[4];
        adv_data_poweron[30] = bond_info.address.addr[3];
        memcpy(adv_buf, adv_data_poweron, sizeof(adv_data_poweron));

    } else if (adv_pkt == LE_REMOTE_PKT_POWEROFF) {
        //按键关机发关机广播，根据实际方案确定是否要发
        data_len = sizeof(adv_data_const_poweroff);
        memcpy(adv_buf, adv_data_const_poweroff, sizeof(adv_data_const_poweroff));

    } else if (adv_pkt == LE_REMOTE_PKT_PAIR) {
        //配对广播
        data_len = sizeof(adv_data_const_pair);
        memcpy(adv_buf, adv_data_const_pair, sizeof(adv_data_const_pair));

    }

    return data_len;
}

void ble_adv_param_init(LE_REMOTE_ADV_PKT adv_pkt)
{
    adv_param_t adv_info;
    memset(&adv_info,0,sizeof(adv_info));
    adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data), adv_pkt);
    adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
    adv_info.adv_int_min = adv_info.adv_int_max = 0x30;
    adv_info.adv_type = ADV_TYPE_IND;
    adv_info.adv_en = 1;
    adv_info.channel_map = ADV_ALL_CHNLS_EN;
    adv_info.filter_policy = LE_ADV_ALLOW_SCAN_ANY_CON_ANY;

    #if LE_WLIST_EN
    sm_bonding_info_t bond_info;
    if (ble_get_bond_info(0, &bond_info)) {
        //bonding info index 0
        printf("%s, add to white list\n", __func__);
        wlist_param_t wlist;
        wlist.size = 1;
        adv_info.filter_policy = LE_ADV_ALLOW_SCAN_WLST_CON_WLST;

        wlist.addr.addr_type = bond_info.address_type;
        memcpy(&wlist.addr.addr, &bond_info.address, 6);
        printf("addr_type = %x, addr:", wlist.addr.addr_type);
        print_r(&wlist.addr.addr, 6);
        ble_add_dev_to_white_list(&wlist);
        delay_5ms(1);
    }
    #endif

    ble_set_adv_param(&adv_info);
}

void ble_adv_ctl_set(uint8_t key_msg)
{
    uint8_t conn_addr[6];

    if ((key_msg == KEY_MSG_POWER) && !ble_cb.con_handle) {
        ble_adv_param_init(LE_REMOTE_PKT_POWERON);

    } else if ((key_msg == KEY_MSG_POWER) && ble_cb.con_handle) {
        ble_adv_param_init(LE_REMOTE_PKT_POWEROFF);

    } else if (key_msg == KEY_MSG_PAIR) {
        ble_adv_dis();
        if (ble_get_bonding_cnt()) {
            ble_delete_bonding_all();
            memset(conn_addr, 0, sizeof(conn_addr));
        }
        if (ble_cb.con_handle) {
            ble_disconnect(ble_cb.con_handle);
            delay_5ms(400);  //2s后发配对广播,根据需求修改
        }
        ble_adv_param_init(LE_REMOTE_PKT_PAIR);
    }
}

AT(.text.app.proc.ble)
void ble_adv_proc(void)
{

}
