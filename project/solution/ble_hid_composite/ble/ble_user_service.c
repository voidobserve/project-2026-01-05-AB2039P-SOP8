#include "include.h"
#include "ble_service.h"
#include "ble_user_service.h"


#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#define TRACE_R(...)            my_print_r(__VA_ARGS__)
#else // !TRACE_EN
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


static att_service_handler_t    generic_access_service;

static att_service_handler_t    generic_attribute_service;
static uint16_t                 generic_attribute_client_config;

#define BLE_CMD_BUF_LEN     4
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)
#define BLE_RX_BUF_LEN      20


struct ble_cmd_t{
    u8 len;
    u8 buf[BLE_RX_BUF_LEN];
    uint16_t handle;
};

struct ble_user_cb_t {
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
} ble_user_cb;

static void user_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    bool event_valid = true;

    switch(event_type){
        case BLE_EVT_CONNECT:{
            if (ble_cb.con_handle == 0) {
                printf("-->BLE_EVENT_CONNECTED: 0x%04x\n", GET_LE16(param + 7));
            }
            memcpy(&ble_cb.con_handle, &param[7], 2);
            ble_disable_latency(ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            if (ble_cb.con_handle) {
                printf("-->BLE_EVENT_DISCONNECTED: 0x%04x\n", ble_cb.con_handle);
            }
            ble_cb.con_handle = 0;
            generic_attribute_client_config = 0;
        } break;

        default:
            event_valid = false;
            break;
    }
    
    if (event_valid) {
        ble_set_service_pending(1);
    }
}

static uint16_t user_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if (attribute_handle == ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE) {
        u8 read_data[] = BLE_DEFAULT_DEVICE_NAME;
        u8 read_data_len = strlen(xcfg_cb.le_name);
        if (read_data_len > 0) {
            if (buffer) {
                memcpy(buffer, xcfg_cb.le_name, read_data_len);
            }
        } else {
            read_data_len = sizeof(read_data) - 1;
            if (buffer){
                memcpy(buffer, read_data, read_data_len);
                TRACE("user_service_read_callback 0x%x data[%d]: ", attribute_handle, buffer_size);
                TRACE_R(buffer, buffer_size);
            }
        }

        ble_set_service_pending(1);
        return read_data_len;
    }

	return 0;
}

static int user_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    TRACE("user_service_write_callback 0x%x data[%d]: ", attribute_handle, buffer_size);
    TRACE_R(buffer, buffer_size);

    if (PEER_DEVICE_TYPE_IOS == ble_hid_peer_device_type_get()) {
        if(attribute_handle == ATT_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_CLIENT_CONFIGURATION_HANDLE){
            generic_attribute_client_config = GET_LE16(&buffer[0]);
            if (generic_attribute_client_config) {
            #if LE_SERVICE_CHANGED
                uint8_t buff[]={0x01, 0x00, 0xff, 0xff};        //handle range : 0x0001 - 0xffff
                ble_indicate_for_handle(ble_cb.con_handle, ATT_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_VALUE_HANDLE, buff, 4);
            #endif
            }
            ble_set_service_pending(1);
        }
    }

	return ATT_ERR_NO_ERR;
}

void ble_user_service_init(void)
{
    printf("%s\n", __func__);

    // register service with ATT Server
	generic_access_service.start_handle   = ATT_SERVICE_GAP_SERVICE_START_HANDLE;
	generic_access_service.end_handle     = ATT_SERVICE_GAP_SERVICE_END_HANDLE;
	generic_access_service.read_callback  = &user_service_read_callback;
	generic_access_service.write_callback = &user_service_write_callback;
	generic_access_service.event_handler  = &user_service_event_callback;
	att_server_register_service_handler(&generic_access_service);

    // register service with ATT Server
	generic_attribute_service.start_handle   = ATT_SERVICE_GATT_SERVICE_START_HANDLE;
	generic_attribute_service.end_handle     = ATT_SERVICE_GATT_SERVICE_END_HANDLE;
	generic_attribute_service.read_callback  = &user_service_read_callback;
	generic_attribute_service.write_callback = &user_service_write_callback;
	generic_attribute_service.event_handler  = NULL;
	att_server_register_service_handler(&generic_attribute_service);

	generic_attribute_client_config = 0;
}

void ble_user_service_proc(void)
{
}
