#include "include.h"
#include "ble_battery_service.h"

static att_service_handler_t    battery_service;
static uint16_t                 battery_level_client_config;
static uint16_t                 battery_power_client_config;
static uint16_t                 battery_connect_handle;

static void battery_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&battery_connect_handle, &param[7], 2);
            //printf("-->BLE_EVENT_CONNECTED:%x\n",battery_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            //uint16_t con_handle;
            //memcpy(&con_handle, &param[0], 2);
            //printf("-->BLE_EVENT_DISCONNECTED:%x\n",con_handle);
            battery_connect_handle = 0;
            battery_power_client_config = CCCD_DFT;
            battery_level_client_config = CCCD_DFT;
        } break;

        default:
            break;
    }
}

static uint16_t battery_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("battery_service_read_callback\n");

    if (attribute_handle == ATT_CHARACTERISTIC_2A19_01_VALUE_HANDLE){
#if BSP_VBAT_DETECT_EN
        u8 read_data[] = {bsp_get_vbat_level(0)};
#else
        u8 read_data[] = {100};
#endif // BSP_VBAT_DETECT_EN
        if (buffer){
            memcpy(buffer, read_data, sizeof(read_data));
        }
        return sizeof(read_data);
    }

    if (attribute_handle == ATT_CHARACTERISTIC_2A1A_01_VALUE_HANDLE){
        u8 read_data[] = {0xaf};
        if (buffer){
            memcpy(buffer, read_data, sizeof(read_data));
        }
        return sizeof(read_data);
    }

    return 0;
}



static int battery_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("battery_service_write_callback\n");

    if (attribute_handle == ATT_CHARACTERISTIC_2A19_01_CLIENT_CONFIGURATION_HANDLE){
        battery_level_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2A1A_01_CLIENT_CONFIGURATION_HANDLE) {
        battery_power_client_config = GET_LE16(&buffer[0]);
    }

    return ATT_ERR_NO_ERR;
}

void ble_battery_service_init(void)
{
    printf("ble_battery_service_init\n");

    // register service with ATT Server
    battery_service.start_handle   = ATT_SERVICE_180F_START_HANDLE;
    battery_service.end_handle     = ATT_SERVICE_180F_END_HANDLE;
    battery_service.read_callback  = &battery_service_read_callback;
    battery_service.write_callback = &battery_service_write_callback;
    battery_service.event_handler  = &battery_service_event_callback;
    att_server_register_service_handler(&battery_service);

    battery_power_client_config = CCCD_DFT;
    battery_level_client_config = CCCD_DFT;
}

void ble_battery_service_proc(void)
{
    static u32 battery_check_timeout;
    static u8 battery_level_last;
    u8 battery_level;
    static u8 power_state = 0;
    u8 charge_sta;

    charge_sta = 0/* External Check */;

    if (tick_check_expire(battery_check_timeout, 1000)) {
        battery_check_timeout = tick_get();

#if BSP_VBAT_DETECT_EN
        battery_level = bsp_get_vbat_level(0);
#else
        battery_level = 100;
#endif // BSP_VBAT_DETECT_EN

        if (!(battery_level - battery_level_last < 10 && battery_level_last - battery_level < 10)) {
            if (battery_level_client_config) {
                ble_notify_for_handle(battery_connect_handle, ATT_CHARACTERISTIC_2A19_01_VALUE_HANDLE, &battery_level, 1);
                battery_level_last = battery_level;
            }
        }

        if (power_state != charge_sta) {
            power_state = charge_sta;
            u8 val;
            if (power_state) {
                val = 0xBB;
            } else {
                val = 0xAE;
            }

            if (battery_power_client_config) {
                ble_notify_for_handle(battery_connect_handle, ATT_CHARACTERISTIC_2A1A_01_VALUE_HANDLE, &val, 1);
            }
        }
    }
}
