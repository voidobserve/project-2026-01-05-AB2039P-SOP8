/**
******************************************************************************************************************************
*
*@file ble_app.c
*
*@brief Source file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "ble_app.h"
#include "ble_fota_service.h"

uint8_t pre_remote_sno = 0;

ble_slave_tx_t ble_slave_tx;

extern void ble_adv_param_init(void);

void ble_cmd_tx_init(void)
{
    memset(&ble_slave_tx, 0, sizeof(ble_slave_tx));
}

bool ble_is_tx_busy(void)
{
    return (0 != ble_slave_tx.ble_cmd_tx_cnt);
}

bool ble_slave_tx_command_ll(uint8_t *param, uint8_t len, uint8_t retry_cnt)
{
    if(!ble_is_tx_busy()){
        #if 1
        adv_param_t adv_info;
        memset(&adv_info, 0, sizeof(adv_info));
        adv_param_t *p = &adv_info;

        ble_slave_tx.ble_cmd_tx_cnt = RETRY_CNT_CHECK(retry_cnt);

        if (ble_slave_tx.ble_cmd_tx_cnt != 0) {
            p->adv_en = 1;
            p->adv_len = len;
            memcpy(p->adv_data, param, p->adv_len);

            p->channel_map = ADV_ALL_CHNLS_EN;
            p->adv_type = ADV_TYPE_NONCONN_IND;
            p->adv_int_min = p->adv_int_max = 0x30;

            ble_set_adv_param(&adv_info);
            return true;
        } else {
            return false;
        }
        #endif

    }

    return false;
}

/*Ble tx time = 30ms * ble_cmd_tx_cnt*/
void ble_slave_tx_proc(void)
{
    static u32 ble_tx_tick = 0;
    if(tick_check_expire(ble_tx_tick, 30) && ble_is_tx_busy()){
        ble_tx_tick = tick_get();

        ble_slave_tx.ble_cmd_tx_cnt --;

        /*recover ADV_IND advertising params.*/
        if (!ble_slave_tx.ble_cmd_tx_cnt) {
            adv_param_t adv_info;

            memset(&adv_info, 0, sizeof(adv_param_t));
            adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data));
            adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
            adv_info.adv_int_min = adv_info.adv_int_max = 0x30;
            adv_info.adv_type = ADV_TYPE_IND;
            adv_info.channel_map = ADV_ALL_CHNLS_EN;

            ble_set_adv_param(&adv_info);
        }
    }
}

void ble_app_env_init(void)
{
    ble_cmd_tx_init();
}



#if LE_SCAN_EN

#if (DEV_TYPE == REMOTE_TYPE_MUSIC_CONTROLLER)
void ble_remote_controller_handler(remote_beacon_t *beacon)
{
    printf("cid %x, sno %x, cmd %x\n", beacon->cid, beacon->sno, beacon->cmd);

    switch (beacon->cmd) {
        case REMOTE_CMD_OFF:
            break;
        case REMOTE_CMD_AUTO:
            break;
        case REMOTE_CMD_ON:
            break;
        case REMOTE_CMD_1:
            break;
        case REMOTE_CMD_2:
            break;
        case REMOTE_CMD_3:
            break;
        case REMOTE_CMD_1H:
            break;
        case REMOTE_CMD_2H:
            break;
        case REMOTE_CMD_3H:
            break;
        case REMOTE_CMD_SPEED_INC:
            break;
        case REMOTE_CMD_SPEED_DEC:
            break;
        case REMOTE_CMD_MODE_INC:
            break;
        case REMOTE_CMD_MODE_DEC:
            break;
        case REMOTE_CMD_BRIGHTNESS_INC:
            break;
        case REMOTE_CMD_BRIGHTNESS_DEC:
            break;
        case REMOTE_CMD_R:
            break;
        case REMOTE_CMD_G:
            break;
        case REMOTE_CMD_B:
            break;
        case REMOTE_CMD_YELLOW:
            break;
        case REMOTE_CMD_GREEN:
            break;
        case REMOTE_CMD_BLUE:
            break;
        case REMOTE_CMD_RED:
            break;
        case REMOTE_CMD_PURPLE:
            break;
        case REMOTE_CMD_WHITE:
            break;
        default:
            break;
    }
}


#elif (DEV_TYPE == REMOTE_TYPE_LAMP_CONTROLLER)
void ble_remote_controller_handler(remote_beacon_t *beacon)
{
    printf("cid %x, sno %x, cmd %x, param %x\n", beacon->cid, beacon->sno, beacon->cmd, beacon->param);

    switch (beacon->cmd) {
        case REMOTE_CMD_BRIGHTNESS_PLUS:
            break;
        case REMOTE_CMD_BRIGHTNESS_MINUS:
            break;
        case REMOTE_CMD_MUSIC:
            break;
        case REMOTE_CMD_ONOFF:
            break;
        case REMOTE_CMD_R:
            break;
        case REMOTE_CMD_G:
            break;
        case REMOTE_CMD_B:
            break;
        case REMOTE_CMD_W:
            break;
        case REMOTE_CMD_PALE_RED:
            break;
        case REMOTE_CMD_PALE_GREEN:
            break;
        case REMOTE_CMD_PALE_BLUE:
            break;
        case REMOTE_CMD_FLASH:
            break;
        case REMOTE_CMD_ORANGE:
            break;
        case REMOTE_CMD_INDIGO:
            break;
        case REMOTE_CMD_PURPLE:
            break;
        case REMOTE_CMD_STROBE:
            break;
        case REMOTE_CMD_ORANGE_YELLOW:
            break;
        case REMOTE_CMD_INDIGO_BLUE:
            break;
        case REMOTE_CMD_BLUE_PURPLE:
            break;
        case REMOTE_CMD_FADE:
            break;
        case REMOTE_CMD_YELLOW:
            break;
        case REMOTE_CMD_INDIGO_GREEN:
            break;
        case REMOTE_CMD_PALE_PURPLE:
            break;
        case REMOTE_CMD_SMOOTH:
            break;
        default:
            break;
    }
}

#endif

void ble_remote_handler(remote_beacon_t *beacon)
{
    //printf("filter_code %08x, cid %04x, sno %02x, cmd %x\n", beacon->filter_code, beacon->cid, beacon->sno, beacon->cmd);

    if (VENDOR_CID != beacon->cid) {
        return;
    }

    if (REMOTE_FILTER_CODE != beacon->filter_code) {
        return;
    }

    // beacon filter
    if (beacon->sno == pre_remote_sno) {
        return;
    }

    ble_remote_controller_handler(beacon);

    pre_remote_sno = beacon->sno;
}

void ble_beacon_analysis(uint8_t addr_type, uint8_t *addr, uint8_t adv_data_len, const uint8_t *adv_data, char rssi)
{
    for (uint8_t offset = 0; offset < adv_data_len - 1;) {
        ble_ltv_t *beacon_ltv = (ble_ltv_t *)&adv_data[offset];

        remote_beacon_t *beacon = (remote_beacon_t *)beacon_ltv->data;

        ble_remote_handler(beacon);

        offset += beacon_ltv->len + 1;
    }
}

void ble_advertising_report_callback(uint8_t evt_type, uint8_t addr_type, uint8_t *addr, uint8_t adv_data_len, const uint8_t *adv_data, char rssi)
{
    if (addr[0] == 0xc5 && addr[1] == 0x04) {
    printf("addr_type %x, Adv Addr:", addr_type);
    print_r(addr, 6);
    //printf("Adv Data: %d###", adv_data_len);
    //print_r(adv_data, adv_data_len);
    }


    #if (DEV_TYPE == REMOTE_TYPE_MUSIC_CONTROLLER)
    ble_beacon_analysis(addr_type, addr, adv_data_len, adv_data, rssi);
    /*2.4G brocast packet emitted by this remote does not follow Bluetooth, need to handle specially.*/
    #elif (DEV_TYPE == REMOTE_TYPE_LAMP_CONTROLLER)
    remote_beacon_t *beacon = (remote_beacon_t *)adv_data;

    ble_remote_handler(beacon);
    #endif
}
#endif

void ble_slave_tx_command(uint8_t *p, uint8_t len, uint8_t retry_cnt)
{
    ble_slave_tx_command_ll(p, len, retry_cnt);
}

