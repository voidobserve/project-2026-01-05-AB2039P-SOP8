#include "include.h"
#include "ble_service.h"

static att_service_handler_t       ff20_service;
static uint16_t ff22_client_config;

#define BLE_CMD_BUF_LEN     4
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)
#define BLE_RX_BUF_LEN      20

struct ble_cmd_t{
    u8 len;
    u8 buf[BLE_RX_BUF_LEN];
    uint16_t handle;
};

struct ble_cb_t {
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
    bool pending;
} ble_user_cb;

AT(.com_sleep.ble.sleep)
bool ble_user_service_pending(void)
{
    return ble_user_cb.pending;
}

static void ble_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&ble_cb.con_handle, &param[7], 2);
            printf("-->BLE_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint16_t con_handle;
            uint8_t disc_reason = param[2];
            memcpy(&con_handle, &param[0], 2);
            printf("-->BLE_EVENT_DISCONNECTED:%x, disc_reason %x\n",con_handle, disc_reason);
            ble_cb.con_handle = 0;
            ff22_client_config = CCCD_DFT;
        } break;

#if LE_SCAN_EN
        case BLE_EVT_ADV_REPORT: {
            //printf(".");
            adv_report_t adv_report;
            adv_report.offset = 0;
            memcpy(&adv_report, param, 10);
            adv_report.report = &param[10];

            ble_advertising_report_callback(adv_report.event_type, adv_report.addr_type, adv_report.addr,
                                            adv_report.report_len, adv_report.report, adv_report.rssi);
        } return;
#endif

        default:
            break;
    }
}

static uint16_t service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("service_read_callback\n");
    u8 report_len;

    if(attribute_handle == ATT_CHARACTERISTIC_FF21_01_VALUE_HANDLE){
        u8 read_data[] = "hello";
        report_len = sizeof(read_data);

        if(buffer){
            u8 data_len = (buffer_size < (report_len - offset))? buffer_size: (report_len - offset);
            memcpy(buffer, read_data + offset, data_len);

            return data_len;
        }
        return report_len;
    }

	return 0;
}

static int service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("service_write_callback\n");

    if(attribute_handle == ATT_CHARACTERISTIC_FF22_01_CLIENT_CONFIGURATION_HANDLE){
        ff22_client_config = GET_LE16(&buffer[0]);
    }else if(attribute_handle == ATT_CHARACTERISTIC_FF21_01_VALUE_HANDLE){
        printf("BLE RX [%d]: \n", buffer_size);
        print_r(buffer, buffer_size);

#if BSP_LEDC_EN
        bsp_ws2815_app_set(buffer[0]);
#endif // BSP_LEDC_EN

        u8 wptr = ble_user_cb.cmd_wptr & BLE_CMD_BUF_MASK;
        ble_user_cb.cmd_wptr++;
        if (buffer_size > BLE_RX_BUF_LEN) {
            buffer_size = BLE_RX_BUF_LEN;
        }
        memcpy(ble_user_cb.cmd[wptr].buf, buffer, buffer_size);
        ble_user_cb.cmd[wptr].len = buffer_size;
        ble_user_cb.cmd[wptr].handle = attribute_handle;
    }

	return ATT_ERR_NO_ERR;
}

void ble_user_service_init(void)
{
    printf("ble_user_service_init\n");

    // get service handle range
	uint16_t start_handle = ATT_SERVICE_FF20_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_FF20_END_HANDLE;

    // register service with ATT Server
	ff20_service.start_handle   = start_handle;
	ff20_service.end_handle     = end_handle;
	ff20_service.read_callback  = &service_read_callback;
	ff20_service.write_callback = &service_write_callback;
	ff20_service.event_handler  = &ble_event_callback;
	att_server_register_service_handler(&ff20_service);

	ff22_client_config = CCCD_DFT;
}

void ble_user_service_proc(void)
{
    if (ble_user_cb.cmd_rptr == ble_user_cb.cmd_wptr) {
        return;
    }
    u8 rptr = ble_user_cb.cmd_rptr & BLE_CMD_BUF_MASK;
    ble_user_cb.cmd_rptr++;
    u8 *ptr = ble_user_cb.cmd[rptr].buf;
    u8 len = ble_user_cb.cmd[rptr].len;
    //uint16_t handle = ble_cb.cmd[rptr].handle;

    if(ff22_client_config){
        printf("BLE TX [%d]: \n", len);
        print_r(ptr, len);
        ble_notify_for_handle(ble_cb.con_handle, ATT_CHARACTERISTIC_FF22_01_VALUE_HANDLE, ptr, len);
    }
}
