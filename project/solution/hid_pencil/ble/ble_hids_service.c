#include "include.h"
#include "ble_hids_service.h"

#define TRACE_CMD_EN            1

#if TRACE_CMD_EN
#define TRACE_CMD(...)          printf("[CMD] "); \
                                printf(__VA_ARGS__)
#else
#define TRACE_CMD(...)
#endif

static att_service_handler_t    hids_service;
static uint16_t                 hids_report_1_client_config;
static uint16_t                 hids_report_2_client_config;
static uint16_t                 hids_connect_handle;

static u8 hid_report_map[] = {
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x01,
    0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00,
    0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x08, 0x81, 0x03, 0x95, 0x05,
    0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x05,
    0x91, 0x02, 0x95, 0x01, 0x75, 0x03, 0x91, 0x03,
    0x95, 0x06, 0x75, 0x08, 0x15, 0x00, 0x26, 0xFF,
    0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0xFF, 0x81,
    0x00, 0xC0, 0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01,
    0x85, 0x02, 0x15, 0x00, 0x26, 0x3C, 0x04, 0x19,
    0x00, 0x2A, 0x3C, 0x04, 0x75, 0x10, 0x95, 0x01,
    0x81, 0x00, 0xC0,
};

volatile u16 key_event = 0;

static void hids_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&hids_connect_handle, &param[7], 2);
            printf("-->BLE_EVENT_CONNECTED:%x\n",hids_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint16_t con_handle;
            uint8_t disc_reason = param[2];
            memcpy(&con_handle, &param[0], 2);
            printf("-->BLE_EVENT_DISCONNECTED:%x, disc_reason %x\n",con_handle, disc_reason);
            hids_connect_handle = 0;
            hids_report_1_client_config = CCCD_DFT;
            hids_report_2_client_config = CCCD_DFT;
        } break;

        default:
            break;
    }
}

static uint16_t hids_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("hids_service_read_callback\n");
    u8 hid_report_map_len = sizeof(hid_report_map);
    u8 data_len = 0;

    if (attribute_handle == ATT_CHARACTERISTIC_2A4B_01_VALUE_HANDLE) {
        if (buffer) {
            data_len = (buffer_size < (hid_report_map_len - offset))? buffer_size: (hid_report_map_len - offset);
            memcpy(buffer, hid_report_map + offset, data_len);
            return data_len;
        }
        return hid_report_map_len;
    }

    return 0;
}


static int hids_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("hids_service_write_callback\n");

    if(attribute_handle == ATT_CHARACTERISTIC_2A4D_01_CLIENT_CONFIGURATION_HANDLE){
        hids_report_1_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2A4D_02_CLIENT_CONFIGURATION_HANDLE) {
        hids_report_2_client_config = GET_LE16(&buffer[0]);
    }

    return ATT_ERR_NO_ERR;
}

static void ble_hid_send_key(u8 modifed, u16 keyboard, u8 hold)
{
    u8 key_buf[8];
    u8 offset = 2 + ((keyboard & 0xff00) >> 8);

    memset(key_buf, 0, 8);
    key_buf[0] |= modifed;
    key_buf[offset] = keyboard & 0xff;

    if (hids_report_1_client_config) {
        ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, key_buf, 8);
        if (!hold) {
            memset(key_buf + 1, 0, 7);  // release key
            ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, key_buf, 8);
            if (modifed) {
                memset(key_buf, 0, 1);  // release modifed
                ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, key_buf, 8);
            }
        }
    }
}

static void ble_hid_send_consumer_cmd(u16 cmd, u8 hold)
{
    u8 cmd_buf[2];

    memset(cmd_buf, 0, 2);
    cmd_buf[0] = cmd & 0xff;
    cmd_buf[1] = (cmd & 0xff00) >> 8;

    if (hids_report_2_client_config) {
        ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2A4D_02_VALUE_HANDLE, cmd_buf, sizeof(cmd_buf));
        if (!hold) {
            memset(cmd_buf, 0, 2);
            ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2A4D_02_VALUE_HANDLE, cmd_buf, sizeof(cmd_buf));
        }
    }
}

void ble_hids_service_init(void)
{
    printf("ble_hids_service_init\n");

    // register service with ATT Server
    hids_service.start_handle   = ATT_SERVICE_1812_START_HANDLE;
    hids_service.end_handle     = ATT_SERVICE_1812_END_HANDLE;
    hids_service.read_callback  = &hids_service_read_callback;
    hids_service.write_callback = &hids_service_write_callback;
    hids_service.event_handler  = &hids_service_event_callback;
    att_server_register_service_handler(&hids_service);

    hids_report_1_client_config = CCCD_DFT;
    hids_report_2_client_config = CCCD_DFT;
}

void ble_hids_service_proc(void)
{
    if (key_event & MSG_SEND_GO_HOME_CMD) {                     // Home
        TRACE_CMD("Go Home\n");
        key_event &= ~MSG_SEND_GO_HOME_CMD;
        ble_hid_send_consumer_cmd(HID_CONSUMER_MENU, 0);
    } else if (key_event & MSG_SEND_MUTITASK_BAR_CMD) {         // 多任务栏
        TRACE_CMD("Go Multi Task\n");
        key_event &= ~MSG_SEND_MUTITASK_BAR_CMD;
        ble_hid_send_consumer_cmd(HID_CONSUMER_MENU, 0);
        ble_hid_send_consumer_cmd(HID_CONSUMER_MENU, 0);
    } else if (key_event & MSG_SEND_SEARCH_BAR) {               // 搜索栏
        TRACE_CMD("Show Search Bar\n");
        key_event &= ~MSG_SEND_SEARCH_BAR;
        ble_hid_send_consumer_cmd(HID_CONSUMER_SEARCH, 0);
    } else if (key_event & MSG_SEND_VOL_INC_CMD) {              // 音量+
        TRACE_CMD("Volume increase\n");
        key_event &= ~MSG_SEND_VOL_INC_CMD;
        ble_hid_send_consumer_cmd(HID_CONSUMER_VOL_INC, 0);
    } else if (key_event & MSG_SEND_VOL_DEC_CMD) {
        TRACE_CMD("Volume decrease\n");
        key_event &= ~MSG_SEND_VOL_DEC_CMD;
        ble_hid_send_consumer_cmd(HID_CONSUMER_VOL_DEC, 0);     // 音量-
    } else if (key_event & MSG_SEND_LOCK_WAKE_SCREEN_CMD) {
        TRACE_CMD("Press Powrer Key\n");
        key_event &= ~MSG_SEND_LOCK_WAKE_SCREEN_CMD;
        ble_hid_send_consumer_cmd(HID_CONSUMER_POWER, 0);
    } else if (key_event & MSG_SEND_UNDO_CMD) {                 // 撤销
        TRACE_CMD("Undo\n");
        key_event &= ~MSG_SEND_UNDO_CMD;
        ble_hid_send_key(HID_LEFT_GUI, HID_KEY_Z, 0);
    } else if (key_event & MSG_SEND_ERASER_SWITCH_CMD) {        // goodnote 等软件切换 [橡皮擦]
        TRACE_CMD("Switch To Eraser (sw.)\n");
        key_event &= ~MSG_SEND_ERASER_SWITCH_CMD;
        ble_hid_send_key(0, HID_KEY_E, 0);
    } else if (key_event & MSG_SEND_PEN_SWITCH_CMD) {           // goodnote 等软件切换 [笔]
        TRACE_CMD("Switch To Pen (sw.)\n");
        key_event &= ~MSG_SEND_PEN_SWITCH_CMD;
        ble_hid_send_key(0, HID_KEY_P, 0);
    }
}
