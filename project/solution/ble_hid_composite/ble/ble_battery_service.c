#include "include.h"
#include "ble_battery_service.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

static att_service_handler_t    battery_service;
static uint16_t                 battery_level_client_config;
static uint16_t                 battery_connect_handle;

static void battery_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&battery_connect_handle, &param[7], 2);
        } break;

        case BLE_EVT_DISCONNECT:{
            battery_connect_handle = 0;
            battery_level_client_config = 0;
        } break;

        default:
            break;
    }
}

static uint16_t battery_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if (attribute_handle == ATT_CHARACTERISTIC_2a19_01_VALUE_HANDLE){
#if BSP_VBAT_DETECT_EN
        u8 read_data[] = {bsp_get_vbat_level(0)};
#else
        u8 read_data[] = {100};
#endif // BSP_VBAT_DETECT_EN
        if (buffer){
            memcpy(buffer, read_data, sizeof(read_data));
            TRACE("battery_service_read_callback 0x%x data[%d]: ", attribute_handle, buffer_size);
            TRACE_R(buffer, buffer_size);
        }
        return sizeof(read_data);
    }

	return 0;
}



static int battery_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    TRACE("battery_service_write_callback 0x%x data[%d]: ", attribute_handle, buffer_size);
    TRACE_R(buffer, buffer_size);

    if (attribute_handle == ATT_CHARACTERISTIC_2a19_01_CLIENT_CONFIGURATION_HANDLE){
        battery_level_client_config = GET_LE16(&buffer[0]);
    }

	return ATT_ERR_NO_ERR;
}

void ble_battery_service_init(void)
{
    printf("%s\n", __func__);

    // register service with ATT Server
	battery_service.start_handle   = ATT_SERVICE_180f_START_HANDLE;
	battery_service.end_handle     = ATT_SERVICE_180f_END_HANDLE;
	battery_service.read_callback  = &battery_service_read_callback;
	battery_service.write_callback = &battery_service_write_callback;
	battery_service.event_handler  = &battery_service_event_callback;
	att_server_register_service_handler(&battery_service);
}

void ble_battery_service_proc(void)
{
     static u32 battery_check_timeout;
     static u8 battery_level_last;
     u8 battery_level;

     u8 client_config_for_android = ((PEER_DEVICE_TYPE_ANDROID == ble_hid_peer_device_type_get()) &&
                                    (battery_connect_handle != 0));

    if (tick_check_expire(battery_check_timeout, 1000)) {
        battery_check_timeout = tick_get();

#if BSP_VBAT_DETECT_EN
        battery_level = bsp_get_vbat_level(0);
#else
        battery_level = 100;
#endif // BSP_VBAT_DETECT_EN
        if (!(battery_level - battery_level_last < 10 && battery_level_last - battery_level < 10)) {
            if (battery_level_client_config || client_config_for_android) {
                ble_notify_for_handle(battery_connect_handle, ATT_CHARACTERISTIC_2a19_01_VALUE_HANDLE, &battery_level, 1);
                battery_level_last = battery_level;
            }
        }
    }
}
