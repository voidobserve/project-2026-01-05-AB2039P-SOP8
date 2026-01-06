#include "include.h"
#include "ble_init.h"


///////////////////////////////////////////////////////////////////////////
#define TX_POOL_SIZE       128


ble_cb_t ble_cb AT(.ble_buf.var);

BLE_WORK_MODE cfg_bt_work_mode            = MODE_NORMAL;


AT(.ble_cache.att)
uint32_t ble_tx_pool[TX_POOL_SIZE / 4 + 3];

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
    txpkt_init(ble_tx_pool, sizeof(ble_tx_pool));
}

void ble_sm_param_init(void)
{
    uint8_t security_req_en = 0;
    uint8_t auth_req = SM_AUTHREQ_BONDING | SM_AUTHREQ_MITM_PROTECTION;
    uint8_t io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT;

    ble_sm_init(security_req_en, io_capability, auth_req);
}

bool ble_get_bond_info(uint8_t index, sm_bonding_info_t* bond_info)
{
    sm_bonding_info_t dev_bind_info;
    bool res = ble_get_bonding_info_for_index(index, &dev_bind_info);

    if (res == false) {
        return false;
    }

    memcpy(bond_info, &dev_bind_info, sizeof(sm_bonding_info_t));

    return (0xff != bond_info->address_type);
}

#if LE_RPA_EN
void ble_rpa_param_init(void)
{
    rpa_param_t rpa_info;

    memset(&rpa_info, 0, sizeof(rpa_param_t));

    rpa_info.rpa_en = 1;
    rpa_info.renew_to = 600;

    ble_set_rslv_renew_to(&rpa_info);
}

void ble_rpa_add_dev_to_list(uint8_t index)
{
    sm_bonding_info_t bond_info;
    if (ble_get_bond_info(index, &bond_info)) {
        // if peer device type is random, we should add to resolvable address list.
        if (bond_info.real_address_type == 1) {
            gap_ral_dev_info_t ral_dev_info;
            ral_dev_info.priv_mode = 1;
            memcpy(ral_dev_info.peer_irk, bond_info.peer_irk.key, 16);
            ral_dev_info.addr.addr_type = bond_info.address_type;
            memcpy(&ral_dev_info.addr.addr, &bond_info.address, 6);
            ble_add_dev_to_ral_list(&ral_dev_info);
            printf("%s, add to ral, addr_type(%x), addr:\n", __func__, ral_dev_info.addr.addr_type);
            print_r(&ral_dev_info.addr.addr, 6);
        }
    }
}
#endif


#if LE_DIR_ADV_EN
void ble_dir_adv_param_init(uint8_t index)
{
    adv_param_t adv_info;
    memset(&adv_info,0,sizeof(adv_info));
    adv_info.adv_int_min = adv_info.adv_int_max = 0x30;
    adv_info.adv_type = ADV_TYPE_DIRECT_LO_IND;
    adv_info.adv_en = 1;
    adv_info.channel_map = ADV_ALL_CHNLS_EN;
    adv_info.filter_policy = LE_ADV_ALLOW_SCAN_ANY_CON_ANY;

    sm_bonding_info_t bond_info;
    if (ble_get_bond_info(index, &bond_info)) {
        adv_info.direct_address_typ = bond_info.address_type;
        memcpy(adv_info.direct_address, bond_info.address.addr, 6);
        printf("%s, dir adv addr(%x):\n", __func__, adv_info.direct_address_typ);
        print_r(adv_info.direct_address, 6);
    }

    ble_set_adv_param(&adv_info);
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
    //ble_adv_param_init();

    #if LE_RPA_EN
    ble_rpa_param_init();
    ble_rpa_add_dev_to_list(0);  //bonding info index 0
    #endif

}

void bsp_ble_init(void)
{
    memset(&ble_cb, 0x00, sizeof(ble_cb));

    btstack_init();

    ble_init_callback_register(ble_periph_init);

    ble_setup();

    #if LE_DIR_ADV_EN
    if (ble_get_bonding_cnt()) {
        printf("ADV_DIR_IND!!\n");
        ble_dir_adv_param_init(0);  //bonding info index 0
    } else
    #endif
    {
        printf("ADV_UNDIR_IND!!\n");
        ble_adv_param_init(LE_REMOTE_PKT_DEFAULT);
    }

#if ALARM_NUMBER
    ble_alarm_init();
#endif
}
