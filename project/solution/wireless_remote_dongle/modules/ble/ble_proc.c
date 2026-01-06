#include "include.h"


#define BLE_CON_SLEEP_INTERVAL    400   //500ms
#define BLE_CON_SLEEP_LATENCY     0
#define BLE_CON_SLEEP_TIMEOUT     1000   //10s

static struct ble_param_t {
    uint16_t conn_interval;
    uint16_t conn_latency;
    uint16_t conn_timeout;
} ble_conn_param;

static void ble_update_param_before_enter_sleep(void)
{
    if (server_info.conn_handle) {
        ble_get_conn_param(server_info.conn_handle, &ble_conn_param.conn_interval, &ble_conn_param.conn_latency, &ble_conn_param.conn_timeout);
        if(ble_conn_param.conn_interval != BLE_CON_SLEEP_INTERVAL){
            ble_update_conn_param(server_info.conn_handle, BLE_CON_SLEEP_INTERVAL, BLE_CON_SLEEP_LATENCY, BLE_CON_SLEEP_TIMEOUT);
        }
    } else {
        ble_scan_dis();
    }
}

static void ble_update_param_after_exit_sleep(void)
{
    if (server_info.conn_handle) {
        ble_update_conn_param(server_info.conn_handle, ble_conn_param.conn_interval, ble_conn_param.conn_latency, ble_conn_param.conn_timeout);
    } else {
        ble_scan_en();
    }
}

void ble_enter_sleep_proc(void)
{
#if ALARM_NUMBER
    ble_alarm_enter_sleep_proc();
#endif
    ble_update_param_before_enter_sleep();
}

void ble_exit_sleep_proc(void)
{
#if ALARM_NUMBER
    ble_alarm_exit_sleep_proc();
#endif
    ble_update_param_after_exit_sleep();
}

void ble_app_proc(void)
{

}
