#include "include.h"
#include "ble_service.h"
#include "ble_battery_service.h"

#define BAT_SVC_DEBUG_EN 1

#if BAT_SVC_DEBUG_EN
#define BAT_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BAT_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_battery_service;

static void ble_battery_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    bool event_valid = true;
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_cb.con_handle, &param[7], 2);
            BAT_SVC_DEBUG("-->BLE_BAT_EVENT_CONNECTED:%x\n",ble_cb.con_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cb.con_handle, &param[0], 2);
            BAT_SVC_DEBUG("-->BLE_BAT_EVENT_DISCONNECTED:%x\n",ble_cb.con_handle);
            ble_cb.con_handle = 0;
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }
}

static uint32_t ble_battery_service_get_bat_level(uint8_t *buf, uint16_t buf_size)
{
    uint32_t ret = 0;
    uint8_t vbat_level[1] = {0};
#if BSP_VBAT_DETECT_EN
        vbat_level[0] = bsp_get_vbat_level(0);
#else
        vbat_level[0] = 100;
#endif // BSP_VBAT_DETECT_EN
    memcpy(buf, vbat_level, buf_size);
    ret = buf_size;
    return ret;
}

static uint16_t ble_battery_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t read_len = 0;
    BAT_SVC_DEBUG("ble battery read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_2A19_01_VALUE_HANDLE:{
            read_len = 1;
            if (buffer == NULL) { //获取长度
                break;
            }
            read_len = ble_battery_service_get_bat_level(buffer, buffer_size);
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }

    // BAT_SVC_DEBUG("len:%d\n", read_len);
    // BAT_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_battery_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t write_len = 0;
    BAT_SVC_DEBUG("ble battery write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_user_set_service_pending();
    }

	return write_len;
}

void ble_battery_service_init(void)
{
    BAT_SVC_DEBUG("ble_battery_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_180F_01_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_180F_01_END_HANDLE;

    // register service with ATT Server
	ble_battery_service.start_handle   = start_handle;
	ble_battery_service.end_handle     = end_handle;
	ble_battery_service.read_callback  = &ble_battery_service_read_callback;
	ble_battery_service.write_callback = &ble_battery_service_write_callback;
	ble_battery_service.event_handler  = &ble_battery_service_event_callback;
	att_server_register_service_handler(&ble_battery_service);

    ble_cb.con_handle = 0;
}

AT(.text.app.proc.ble)
void ble_battery_service_proc(void)
{
    if (ble_cb.con_handle == 0) {
        return;
    }
}
