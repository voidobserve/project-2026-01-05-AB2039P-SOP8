#include "include.h"

///////////////////////////////////////////////////////////////////////////
#define NOTIFY_POOL_SIZE       128

ble_cb_t ble_cb AT(.ble_buf.var);

BLE_WORK_MODE cfg_bt_work_mode            = MODE_NORMAL;

AT(.ble_cache.att)
uint32_t notify_tx_pool[NOTIFY_POOL_SIZE/4 + 3];

void ble_set_work_mode(BLE_WORK_MODE mode)
{
    cfg_bt_work_mode = mode;
}

BLE_WORK_MODE ble_get_work_mode(void)
{
    return cfg_bt_work_mode;
}

void ble_txpkt_init(void)
{
    txpkt_init(notify_tx_pool, sizeof(notify_tx_pool));
}

void ble_sm_param_init(void)
{
    uint8_t security_req_en = 0;
    uint8_t auth_req = SM_AUTHREQ_BONDING | SM_AUTHREQ_MITM_PROTECTION;
    uint8_t io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT;

    ble_sm_init(security_req_en, io_capability, auth_req);
}


#if LE_SCAN_EN
void ble_scan_param_init(void)
{
    scan_param_t param;

    memset(&param, 0, sizeof(scan_param_t));

    param.scan_type = 0;
    param.scan_interval = 0xA0;
    param.scan_window = 0x30;

    ble_set_scan_param(&param);

    ble_scan_en();
}
#endif

void ble_periph_init(void)
{
    ble_txpkt_init();
#if LE_SM_EN
    ble_sm_param_init();
#endif
    gap_random_address_set_mode(GAP_PUBLIC_ADDRESS_TYPE);
    att_server_init(ble_get_profile_data());
    ble_service_init();
    ble_adv_param_init();
#if LE_SCAN_EN
    ble_scan_param_init();
#endif
}

void bsp_ble_init(void)
{
    memset(&ble_cb, 0x00, sizeof(ble_cb));

    btstack_init();

    ble_init_callback_register(ble_periph_init);

    ble_setup();

#if ALARM_NUMBER
    ble_alarm_init();
#endif
}
