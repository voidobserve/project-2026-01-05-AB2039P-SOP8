#include "include.h"
#include "ble_client.h"


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


server_info_t server_info;

static att_client_handler_t  client_handler;

static client_state_t tc_state = STA_IDLE;

static bool ble_adv_report_analyse(adv_report_t *adv_report)
{
    adv_struct_t adv_struct;

    while(ble_adv_report_info_analyse(adv_report, &adv_struct)){
        if(adv_struct.adv_type == 0xff){
            if(adv_struct.data_len == 2){
                if(adv_struct.data[0] == 0x42 && adv_struct.data[1] == 0x06){
                    return true;
                }
            }
        }
    }

    return false;
}

void ble_client_event_callback(uint8_t event_type, uint8_t *packet, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:
            printf("BLE_EVT_CONNECT\n");
            memcpy(&server_info.conn_handle, &packet[7], 2);
            printf("server_info.conn_handle:%x\n",server_info.conn_handle);
            tc_state = STA_W4_SERVICE_RESULT;
            server_info.service_cnt = 0;
            ble_client_discover_primary_services(server_info.conn_handle);
            return;

        case BLE_EVT_DISCONNECT:
            printf("BLE_EVT_DISCONNECT\n");
            server_info.conn_handle = 0;
            return;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE:
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            return;

        case BLE_EVT_ADV_REPORT: {
        //    printf(".\n");
            adv_report_t adv_report;
            adv_report.offset = 0;
            memcpy(&adv_report, packet, 10);
            adv_report.report = &packet[10];
            if (ble_adv_report_analyse(&adv_report)){
                ble_scan_dis();
                printf("ble_connect_by_addr: ");
                print_r(adv_report.addr, 6);
                le_conn_initiate_err_code_t ret = ble_connect_by_addr(adv_report.addr, adv_report.addr_type, 24, 0, 400);
                if (ret) {
                    printf("Central initiate a connecetion failed!, reason = %x\n", ret);
                }
                return;
            }
        } return;
    }

    switch(tc_state){
        case STA_W4_SERVICE_RESULT:
            switch(event_type){
                case BLE_EVT_SERVICE_FIND_RESULT:
                    printf("BLE_EVT_SERVICE_FIND_RESULT\n");
                    if(server_info.service_cnt >= MAX_SERVICE_NUM){
                        printf("error: server_info.service_cnt >= MAX_SERVICE_NUM\n");
                    }else{
                        ble_service_query_result_get_service(packet, &server_info.service[server_info.service_cnt++]);
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    server_info.service_index = 0;
                    server_info.characteristic_index = 0;
                    if(server_info.service_cnt != 0){
                        printf("All service discover complete:%d\n",server_info.service_cnt);
                        for(uint8_t cnt=0;cnt<server_info.service_cnt;cnt++){
                            printf("service[%d]->uuid16:%04x,start handle:%04x,stop handle:%04x\n",cnt,server_info.service[cnt].uuid16,server_info.service[cnt].start_group_handle,server_info.service[cnt].end_group_handle);
                        }
                        printf("Client Discovery characteristic start\n");
                        tc_state = STA_W4_CHARACTERISTIC_RESULT;
                        server_info.characteristic_cnt = 0;
                        ble_client_discover_characteristics_for_service(server_info.conn_handle,&server_info.service[0]);
                    }
                    else{
                        tc_state = STA_IDLE;
                        server_info.service_cnt = 0;
                        server_info.characteristic_cnt = 0;
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
                    if(server_info.characteristic_cnt >= MAX_CHARACTERISTIC_NUM){
                        printf("error: server_info.characteristic_cnt >= MAX_CHARACTERISTIC_NUM\n");
                    }else{
                        ble_characteristic_query_result_get_characteristic(packet,&server_info.characteristic[server_info.characteristic_cnt]);
                        server_info.characteristic_cnt++;
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    if (++server_info.service_index < server_info.service_cnt) {
                        tc_state = STA_W4_CHARACTERISTIC_RESULT;
                        ble_client_discover_characteristics_for_service(server_info.conn_handle,&server_info.service[server_info.service_index]);
                    } else {
                        uint16_t config;
                        while(server_info.characteristic_index < server_info.characteristic_cnt) {
                            if(server_info.characteristic[server_info.characteristic_index].properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE)){
                                printf("notify uuid:%04x\n",server_info.characteristic[server_info.characteristic_index].uuid16);
                                if(server_info.characteristic[server_info.characteristic_index].properties & ATT_PROPERTY_NOTIFY){
                                    config = GATT_CCC_NOTIFICATION;
                                }else{
                                    config = GATT_CCC_INDICATION;
                                }
                                ble_client_write_client_characteristic_configuration(server_info.conn_handle, &server_info.characteristic[server_info.characteristic_index], config);
                                server_info.characteristic_index++;
                                tc_state = STA_W4_CHARACTERISTIC_RESULT;
                                return;
                            }
                            server_info.characteristic_index++;
                        }
                        tc_state = STA_SUBSCRIBED;
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

    return 0;
}

enum {
    REPORT_INPUT = 1,
    REPORT_OUTPUT,
}REPORT_TYPE;

static uint16_t ble_att_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("attribute_handle :%x,%d,report id:%d\n",attribute_handle,buffer_size, buffer[0]);

//    print_r(buffer, buffer_size);

    if(buffer_size == 2) {
        if(buffer[1] == REPORT_INPUT) {
            printf("read report type is input!\r\n");
        }
        else {
            printf("read report type is output!\r\n");
        }
    }
    return 0;
}


void ble_client_init(void)
{
    client_handler.event_handler = ble_client_event_callback;
    client_handler.notify_callback = ble_client_notify_callback;
    client_handler.read_callback = ble_att_read_callback;

    att_client_register_handler(&client_handler);
}


void ble_scan_en_test(void)
{
    if (!server_info.conn_handle && ble_get_scan_sta() != LE_SCAN_STA_ENABLE) {
        printf("ble_scan_en\n");
        ble_scan_en();
    }
}

void ble_write_test(void)
{
   u8 buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
   u8 i = 0;

   while(i < server_info.characteristic_cnt) {
       if((server_info.characteristic[i].properties & ATT_PROPERTY_WRITE) == ATT_PROPERTY_WRITE) {
           printf("write uuid:%04x\n",server_info.characteristic[i].uuid16);
           ble_write_req_for_character(server_info.conn_handle, &server_info.characteristic[i], buf, 5);
           return;
       }
       i++;
   }

//    u16 uuid = 0xff21;
//
//    gatt_client_characteristic_t* characteristic = ble_get_characteristic_for_uuid16(server_info.characteristic, server_info.characteristic_cnt, uuid);
//
//    if(characteristic){
//        ble_write_req_for_character(server_info.conn_handle, characteristic, buf, 5);
//    }
}

void ble_read_test(void)
{
#if 0
    u8 i = 0;
    while(i < server_info.characteristic_cnt) {
       if((server_info.characteristic[i].properties & ATT_PROPERTY_READ) == ATT_PROPERTY_READ) {
           printf("write uuid:%04x\n",server_info.characteristic[i].uuid16);
           ble_read_req_for_character(server_info.conn_handle, &server_info.characteristic[i]);
       //    return;

       }
       i++;
    }
#endif
    u16 uuid = 0x2a4d;

    for(int j=0;j<server_info.characteristic_cnt;j++) {
        if(server_info.characteristic[j].uuid16 == uuid) {
            printf("cc uuid:%x,index:%d,hdl:%x,%x\r\n",server_info.characteristic[j].uuid16,j,server_info.characteristic[j].value_handle, server_info.characteristic[j].end_handle);
            ble_read_req_for_character_report_ref(server_info.conn_handle, &server_info.characteristic[j]);
            delay_ms(100);
        }
    }

}

