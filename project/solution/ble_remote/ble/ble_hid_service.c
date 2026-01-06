#include "include.h"
#include "ble_service.h"
#include "ble_hid_service.h"

#define HID_SVC_DEBUG_EN 1

#if HID_SVC_DEBUG_EN
#define HID_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define HID_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_hid_service;

ble_hid_tx_t ble_hid_tx AT(.ble_hid.tx);
ble_hid_var_t ble_hid_var AT(.ble_hid.var);

static uint16_t kb_input_client_config;
static uint16_t ms_input_client_config;
static uint16_t report_01_client_config;

const uint8_t ble_hid_descriptor[] = {
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x2A, 0x9C, 0x02,  //   Usage Maximum (AC Distribute Vertically)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x9C, 0x02,  //   Logical Maximum (668)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x02,        //   Usage (Numeric Key Pad)
    0xA1, 0x02,        //   Collection (Logical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x0A,        //     Usage Maximum (0x0A)
    0x15, 0x01,        //     Logical Minimum (1)
    0x25, 0x0A,        //     Logical Maximum (10)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x08,        //     Report Size (8)
    0x81, 0x40,        //     Input (Data,Array,Abs,No Wrap,Linear,Preferred State,Null State)
    0xC0,              //   End Collection
    0xC0,              // End Collection

    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x06,        //   Report Count (6)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xA4, 0x00,  //   Logical Maximum (164)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x2A, 0xA4, 0x00,  //   Usage Maximum (0xA4)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection

    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x03,        //     Report ID (3)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x80,        //     Logical Minimum (-128)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x05,        //     Report Count (5)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x03,        //     Report Size (3)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection

    0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
    0x09, 0x01,        // Usage (0x01)
    0xA1, 0x02,        // Collection (Logical)
    0x85, 0x04,        //   Report ID (4)
    0x09, 0x14,        //   Usage (0x14)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x50,        //   Report Count (80)
    0x15, 0x80,        //   Logical Minimum (-128)
    0x25, 0x7F,        //   Logical Maximum (127)
    0x81, 0x22,        //   Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)
    0x85, 0x04,        //   Report ID (4)
    0x09, 0x04,        //   Usage (0x04)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};

const uint8_t ble_hid_information[] = {
    0x11, 0x01,     //Hid Version
    0x00,           //Country Code
    0x03,           //feature
};

static void  ble_hid_report_01_client_config_read(uint8_t *param)
{
    bsp_param_read(param, PARAM_HID_CLIENT_CONFIG, PARAM_HID_CLIENT_CONFIG_LEN);
}

static void ble_hid_report_01_client_config_write(uint8_t *param)
{
    bsp_param_write(param, PARAM_HID_CLIENT_CONFIG, PARAM_HID_CLIENT_CONFIG_LEN);
    bsp_param_sync();
}

static void ble_hid_var_init(void)
{
    memset(&ble_hid_var, 0, sizeof(ble_hid_var_t));
}

static void ble_hid_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    bool event_valid = true;
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_cb.con_handle, &param[7], 2);
            HID_SVC_DEBUG("-->BLE_HID_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cb.con_handle, &param[0], 2);
            HID_SVC_DEBUG("-->BLE_HID_EVENT_DISCONNECTED:%x\n",ble_cb.con_handle);
            ble_cb.con_handle = 0;

            #if LE_DIR_ADV_EN
            if (ble_get_bonding_cnt()) {
                #if LE_RPA_EN
                ble_rpa_param_init();
                ble_rpa_add_dev_to_list(0);  //bonding info index 0
                #endif //LE_RPA_EN
                ble_dir_adv_param_init(0);  //bonding info index 0
            }
            #endif
        } break;

        case BLE_EVT_PAIRING_START:{
            HID_SVC_DEBUG("-->BLE_EVT_PAIRING_START\n");
        } break;

        case BLE_EVT_PAIRING_DONE:{
            HID_SVC_DEBUG("-->BLE_EVT_PAIRING_DONE status:0x%x\n", param[9]);  //status 0x00 is pairing success
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
}

static uint16_t ble_hid_service_read_cb_proc(uint16_t data_size, const uint8_t *data, uint16_t offset, uint8_t *read_buf, uint16_t read_buf_size)
{
    uint16_t read_size = data_size;
    read_size = read_size - offset;
    read_size = (read_size > read_buf_size) ? read_buf_size : read_size;
    memcpy(read_buf, data+offset, read_size);
    return read_size;
}

static uint16_t ble_hid_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t read_len = 0;
    HID_SVC_DEBUG("hid read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch(attribute_handle){
        case ATT_CHARACTERISTIC_2A4A_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_information);
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len = ble_hid_service_read_cb_proc(read_len, ble_hid_information, offset, buffer, buffer_size);
        } break;

        case ATT_CHARACTERISTIC_2A4B_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_descriptor);
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len = ble_hid_service_read_cb_proc(read_len, ble_hid_descriptor, offset, buffer, buffer_size);
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }

    // HID_SVC_DEBUG("len:%d\n", read_len);
    // HID_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_hid_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
	bool event_valid = true;
    HID_SVC_DEBUG("hid write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A22_01_CLIENT_CONFIGURATION_HANDLE:
            kb_input_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_2A33_01_CLIENT_CONFIGURATION_HANDLE:
            ms_input_client_config = GET_LE16(&buffer[0]);
            break;

        case ATT_CHARACTERISTIC_2A4D_01_CLIENT_CONFIGURATION_HANDLE:
            report_01_client_config = GET_LE16(&buffer[0]);
            ble_hid_report_01_client_config_write((uint8_t *)&report_01_client_config);
            break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }

	return ATT_ERR_NO_ERR;
}

void ble_hid_service_notify_for_handle(uint16_t att_handle, uint32_t cmd)
{
    if(report_01_client_config){
        memcpy(ble_hid_tx.buf, &cmd, BLE_HID_NOTIFY_DATA_BUF_LEN);
        ble_notify_for_handle(ble_cb.con_handle, att_handle, ble_hid_tx.buf, BLE_HID_NOTIFY_DATA_BUF_LEN);
    }
}

void ble_hid_service_init(void)
{
    HID_SVC_DEBUG("ble_ble_hid_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_1812_01_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_1812_01_END_HANDLE;

    // register service with ATT Server
	ble_hid_service.start_handle   = start_handle;
	ble_hid_service.end_handle     = end_handle;
	ble_hid_service.read_callback  = &ble_hid_service_read_callback;
	ble_hid_service.write_callback = &ble_hid_service_write_callback;
	ble_hid_service.event_handler  = &ble_hid_service_event_callback;
	att_server_register_service_handler(&ble_hid_service);

    ble_hid_report_01_client_config_read((uint8_t *)&report_01_client_config);
    ble_hid_var_init();
}

void ble_hid_key_notify_do(KEY_MSG_TYPEDEF key_msg)
{
    uint32_t hid_key_cmd = BLE_HID_KEY_NONE_CMD;

    switch (key_msg) {
        case KEY_MSG_RELEASE:
            hid_key_cmd = BLE_HID_KEY_RELEASE_CMD;
            break;
        case KEY_MSG_UP:
            hid_key_cmd = BLE_HID_KEY_UP_CMD;
            break;
        case KEY_MSG_DOWN:
            hid_key_cmd = BLE_HID_KEY_DOWN_CMD;
            break;
        case KEY_MSG_LEFT:
            hid_key_cmd = BLE_HID_KEY_LEFT_CMD;
            break;
        case KEY_MSG_RIGHT:
            hid_key_cmd = BLE_HID_KEY_RIGHT_CMD;
            break;
        case KEY_MSG_MEUN:
            hid_key_cmd = BLE_HID_KEY_MEUN_CMD;
            break;
        case KEY_MSG_HOME:
            hid_key_cmd = BLE_HID_KEY_HOME_CMD;
            break;
        case KEY_MSG_OK:
            hid_key_cmd = BLE_HID_KEY_OK_CMD;
            break;
        case KEY_MSG_BACK:
            hid_key_cmd = BLE_HID_KEY_BACK_CMD;
            break;
        case KEY_MSG_VOL_UP:
            hid_key_cmd = BLE_HID_KEY_VOL_UP_CMD;
            break;
        case KEY_MSG_VOL_DOWN:
            hid_key_cmd = BLE_HID_KEY_VOL_DOWN_CMD;
            break;
        case KEY_MSG_POWER:
            hid_key_cmd = BLE_HID_KEY_PWR_CMD;
            break;

        default:
            break;
    }

    if (hid_key_cmd != BLE_HID_KEY_NONE_CMD) {
        bsp_key_set_msg(KEY_MSG_NONE);
        ble_hid_service_notify_for_handle(ATT_CHARACTERISTIC_2A4D_01_VALUE_HANDLE, hid_key_cmd);
    }
}

AT(.text.app.proc.ble)
void ble_hid_service_proc(void)
{
    if (ble_cb.con_handle == 0) {
        return;
    }

    KEY_MSG_TYPEDEF key_msg = bsp_key_get_msg();

    if (key_msg != KEY_MSG_NONE){
        lowpwr_sleep_delay_reset();
        ble_hid_key_notify_do(key_msg);
    }
}
