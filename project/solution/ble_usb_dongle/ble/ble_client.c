#include "include.h"
#include "ble_client.h"
#include "ble_voice.h"
#include "bsp_audio_dec.h"


typedef enum {
    STA_IDLE,
    STA_W4_ENCRYPTED_CONNECTION,
    STA_W4_SERVICE_RESULT,
    STA_W4_CHARACTERISTIC_RESULT,
    STA_W4_DATA_SOURCE_SUBSCRIBED,
    STA_W4_NOTIFICATION_SOURCE_SUBSCRIBED,
    STA_SUBSCRIBED,
    STA_W4_DISCONNECT
} client_state_t;

AT(.ble_buf.svc_info)
static server_info_t server_info[MAX_BLE_CONNECTIONS];

static att_client_handler_t  client_handler;

static client_state_t tc_state[MAX_BLE_CONNECTIONS];

server_info_t *ble_get_server_info(uint8_t conidx)
{
    return &server_info[conidx];
}

void ble_client_event_callback(uint8_t event_type, uint8_t *packet, uint16_t size)
{
    uint8_t conidx = 0;

    switch(event_type){
        case BLE_EVT_CONNECT:
            conidx = ble_get_free_conidx();

            if (conidx != INVALID_CONIDX) {
                ble_cb_t *ble_cb_node = ble_get_ble_info();

                memcpy(&ble_cb_node[conidx].conhdl, &packet[7], 2);

                printf("-->BLE_EVT_CONNECT:%x, conidx %x\n",  ble_cb_node[conidx].conhdl, conidx);

                if (ble_get_conn_cnt() < MAX_BLE_CONNECTIONS) {
                    ble_scan_en();
                }

                memcpy(&server_info[conidx].conn_handle, &packet[7], 2);

                printf("server_info.conn_handle:%x\n",server_info[conidx].conn_handle);
                tc_state[conidx] = STA_W4_SERVICE_RESULT;
                server_info[conidx].service_cnt = 0;

                uint8_t ret = ble_client_discover_primary_services(server_info[conidx].conn_handle);
                printf("sdp result:%x\n", ret);
            } else {
                printf("%s conidx(%d) must less than (%d)\n", conidx, MAX_BLE_CONNECTIONS);
            }
            return;

        case BLE_EVT_DISCONNECT:
            printf("BLE_EVT_DISCONNECT\n");
            uint16_t con_handle;
            uint8_t disc_reason = packet[2];
            memcpy(&con_handle, &packet[0], 2);

            conidx = ble_get_conidx_for_handle(con_handle);

            if (conidx != INVALID_CONIDX) {
                ble_cb_t *ble_cb_node = ble_get_ble_info();

                printf("-->BLE_EVENT_DISCONNECTED:%x, disc_reason %x\n",con_handle, disc_reason);

                ble_cb_node[conidx].conhdl = INVALID_HANDLE;
            } else {
                printf("%s conidx(%d) must less than (%d)\n", conidx, MAX_BLE_CONNECTIONS);
            }
            server_info[conidx].conn_handle = 0;
            return;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE:
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            return;

        case BLE_EVT_ADV_REPORT: {
            adv_report_t adv_report;
            adv_report.offset = 0;
            memcpy(&adv_report, packet, 10);
            adv_report.report = &packet[10];

            ble_advertising_report_callback(adv_report.event_type, adv_report.addr_type, adv_report.addr,
                                            adv_report.report_len, adv_report.report, adv_report.rssi);

        } return;
    }

    uint16_t conhdl = (packet[0] | packet[1] << 8);
    conidx = ble_get_conidx_for_handle(conhdl);
    server_info_t *p_svc_info = &server_info[conidx];

    switch(tc_state[conidx]){
        case STA_W4_SERVICE_RESULT:
            switch(event_type){
                case BLE_EVT_SERVICE_FIND_RESULT:
                    printf("BLE_EVT_SERVICE_FIND_RESULT\n");
                    if(p_svc_info->service_cnt >= MAX_SERVICE_NUM){
                        printf("error: server_info.service_cnt >= MAX_SERVICE_NUM\n");
                    }else{
                        ble_service_query_result_get_service(packet, &p_svc_info->service[p_svc_info->service_cnt++]);
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    p_svc_info->service_index = 0;
                    p_svc_info->characteristic_index = 0;

                    if(p_svc_info->service_cnt != 0){
                        printf("All service discover complete:%d\n", p_svc_info->service_cnt);
                        for(uint8_t cnt = 0; cnt < p_svc_info->service_cnt; cnt++){
                            printf("service[%d]->uuid16:%04x,start handle:%04x,stop handle:%04x\n",cnt, p_svc_info->service[cnt].uuid16, \
                                        p_svc_info->service[cnt].start_group_handle, p_svc_info->service[cnt].end_group_handle);
                        }
                        printf("Client Discovery characteristic start\n");
                        tc_state[conidx] = STA_W4_CHARACTERISTIC_RESULT;
                        p_svc_info->characteristic_cnt = 0;
                        ble_client_discover_characteristics_for_service(p_svc_info->conn_handle, &p_svc_info->service[0]);
                    }
                    else{
                        tc_state[conidx] = STA_IDLE;
                        p_svc_info->service_cnt = 0;
                        p_svc_info->characteristic_cnt = 0;
                        printf("Client Discovery service failed->No service\n");
                    }
                    break;

                default:
                    break;
            }
            break;

        case STA_W4_CHARACTERISTIC_RESULT:
            switch(event_type){
                case BLE_EVT_CHARACTERISTIC_FIND_RESULT:
                    printf(" BLE_EVT_CHARACTERISTIC_FIND_RESULT characteristic_cnt %d\n", p_svc_info->characteristic_cnt);
                    if(p_svc_info->characteristic_cnt >= MAX_CHARACTERISTIC_NUM){
                        printf("error: server_info.characteristic_cnt >= MAX_CHARACTERISTIC_NUM\n");
                    }else{
                        ble_characteristic_query_result_get_characteristic(packet, &p_svc_info->characteristic[p_svc_info->characteristic_cnt]);
                        p_svc_info->characteristic_cnt++;
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    if (++p_svc_info->service_index < p_svc_info->service_cnt) {
                        uint8_t svc_idx = p_svc_info->service_index;
                        tc_state[conidx] = STA_W4_CHARACTERISTIC_RESULT;
                        ble_client_discover_characteristics_for_service(p_svc_info->conn_handle, &p_svc_info->service[svc_idx]);
                    } else {
                        uint16_t config;
                        while(p_svc_info->characteristic_index < p_svc_info->characteristic_cnt) {
                            uint8_t charc_idx = p_svc_info->characteristic_index;
                            if(p_svc_info->characteristic[charc_idx].properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE)){
                                printf("notify uuid:%04x\n", p_svc_info->characteristic[p_svc_info->characteristic_index].uuid16);
                                if(p_svc_info->characteristic[p_svc_info->characteristic_index].properties & ATT_PROPERTY_NOTIFY){
                                    config = GATT_CCC_NOTIFICATION;
                                }else{
                                    config = GATT_CCC_INDICATION;
                                }
                                ble_client_write_client_characteristic_configuration(p_svc_info->conn_handle, &p_svc_info->characteristic[p_svc_info->characteristic_index], config);
                                p_svc_info->characteristic_index++;
                                tc_state[conidx] = STA_W4_CHARACTERISTIC_RESULT;
                                return;
                            }
                            p_svc_info->characteristic_index++;
                        }
                        tc_state[conidx] = STA_SUBSCRIBED;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

static uint16_t ble_client_notify_callback (uint16_t con_handle, uint16_t attribute_handle, uint8_t * buffer, uint16_t buffer_size)
{
    printf("ble_client_notify_callback\n");
    print_r(buffer, buffer_size);

#if LE_VOICE_REMOTE_DONGLE_EN
    ble_voice_remote_cmd_proc(con_handle, attribute_handle, buffer, buffer_size);
    ble_voice_audio_dec_proc(con_handle, attribute_handle, buffer, buffer_size);
#endif //LE_VOICE_REMOTE_DONGLE_EN
    return 0;
}


static uint16_t ble_att_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("attribute_handle:%x\n",attribute_handle);

    print_r(buffer, buffer_size);

    return 0;
}


void ble_client_init(void)
{
    memset(tc_state, STA_IDLE, sizeof(tc_state));
    memset(&server_info, 0, sizeof(server_info));

    client_handler.event_handler = ble_client_event_callback;
    client_handler.notify_callback = ble_client_notify_callback;
    client_handler.read_callback = ble_att_read_callback;

    att_client_register_handler(&client_handler);
}

void ble_app_proc(void)
{

}


//void ble_write_test(void)
//{
//    u8 buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
//    u8 i = 0;
//
//    while(i < server_info.characteristic_cnt) {
//        if((server_info.characteristic[i].properties & ATT_PROPERTY_WRITE) == ATT_PROPERTY_WRITE) {
//            printf("write uuid:%04x\n",server_info.characteristic[i].uuid16);
//            ble_write_req_for_character(server_info.conn_handle, &server_info.characteristic[i], buf, 5);
//            return;
//        }
//        i++;
//    }
//
////    u16 uuid = 0xff21;
////
////    gatt_client_characteristic_t* characteristic = ble_get_characteristic_for_uuid16(server_info.characteristic, server_info.characteristic_cnt, uuid);
////
////    if(characteristic){
////        ble_write_req_for_character(server_info.conn_handle, characteristic, buf, 5);
////    }
//}

//void ble_read_test(void)
//{
////    u8 i = 0;
////
////    while(i < server_info.characteristic_cnt) {
////        if((server_info.characteristic[i].properties & ATT_PROPERTY_READ) == ATT_PROPERTY_READ) {
////            printf("write uuid:%04x\n",server_info.characteristic[i].uuid16);
////            ble_read_req_for_character(server_info.conn_handle, &server_info.characteristic[i]);
////            return;
////        }
////        i++;
////    }
//
//    u16 uuid = 0x2a50;
//
//    gatt_client_characteristic_t* characteristic = ble_get_characteristic_for_uuid16(server_info.characteristic, server_info.characteristic_cnt, uuid);
//
//    if(characteristic){
//        ble_read_req_for_character(server_info.conn_handle, characteristic);
//    }
//}
