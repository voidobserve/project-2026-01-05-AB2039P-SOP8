#include "include.h"
#include "ble_client.h"
#include "ble_user_service.h"

#define MAX_SERVICE_NUM              1

typedef enum {
    STA_IDLE,
    STA_W4_ENCRYPTED_CONNECTION,
    STA_W4_SERVICE_RESULT,
    STA_W4_DISCONNECT
} client_state_t;

typedef struct {
    gatt_client_service_t  service[MAX_SERVICE_NUM];
    uint8_t service_cnt;
    uint8_t service_index;
    uint16_t conn_handle;
} server_info_t;

static server_info_t server_info;

static att_client_handler_t  client_handler;

static client_state_t tc_state = STA_IDLE;

u8 attribute_uuid128[16] = {0x79, 0x05, 0xf4, 0x31, 0xb5, 0xce, 0x4e, 0x99, 0xa4, 0x0f, 0x4b, 0x1e, 0x12, 0x2d, 0x00, 0xd0};

void ble_client_event_callback(uint8_t event_type, uint8_t *packet, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:
            printf("BLE_EVT_CONNECT\n");
            memcpy(&server_info.conn_handle, &packet[7], 2);
            printf("server_info.conn_handle:%x\n",server_info.conn_handle);
            tc_state = STA_W4_ENCRYPTED_CONNECTION;
            return;

        case BLE_EVT_DISCONNECT:
            printf("BLE_EVT_DISCONNECT\n");
            server_info.conn_handle = 0;
            return;
    }

    switch(tc_state){
        case STA_W4_ENCRYPTED_CONNECTION:
            switch (event_type) {
                case BLE_EVT_ENCRYPTION_CHANGE:
                    printf("BLE_EVT_ENCRYPTION_CHANGE\n");
                    tc_state = STA_W4_SERVICE_RESULT;
                    server_info.service_cnt = 0;
                    ble_client_discover_primary_services_by_uuid128(ble_cb.con_handle, attribute_uuid128);
                    break;

                default:
                    break;
            }

        case STA_W4_SERVICE_RESULT:
            switch(event_type){
                case BLE_EVT_SERVICE_FIND_RESULT:
                    printf("BLE_EVT_SERVICE_FIND_RESULT\n");
                    if(server_info.service_cnt >= MAX_SERVICE_NUM){
                        printf("error: server_info.service_cnt >= MAX_SERVICE_NUM\n");
                    }else{
                        printf("Target Primary Service is Discovery.\n");
                        ble_hid_peer_device_type_set(PEER_DEVICE_TYPE_IOS);
                        ble_service_query_result_get_service(packet, &server_info.service[server_info.service_cnt++]);
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    server_info.service_index = 0;
                    if(server_info.service_cnt == 0){
                        server_info.service_cnt = 0;
                        ble_hid_peer_device_type_set(PEER_DEVICE_TYPE_ANDROID);
                        printf("Client Discovery service failed->No service\n");
                    }
                    tc_state = STA_IDLE;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void ble_client_init(void)
{
    printf("%s\n", __func__);

    client_handler.event_handler = ble_client_event_callback;
    client_handler.notify_callback = NULL;
    client_handler.read_callback = NULL;

    att_client_register_handler(&client_handler);
}
