#include "include.h"
#include "ble_fota_service.h"
#include "ble_adv.h"
#include "ble_hid_service.h"
#include "ble_battery_service.h"

// #define BLE_ADV_SLEEP_INTERVAL    800   //500ms
// #define BLE_CON_SLEEP_LATENCY     60

// static struct ble_param_t {
//     uint16_t adv_interval;
//     uint16_t conn_interval;
//     uint16_t conn_latency;
//     uint16_t conn_timeout;
// } ble_param;

// static void ble_update_param_before_enter_sleep(void)
// {
//     if (ble_cb.con_handle) {
//         //uint16_t timeout;
//         ble_enable_latency(ble_cb.con_handle);
//         ble_get_conn_param(ble_cb.con_handle, &ble_param.conn_interval, &ble_param.conn_latency, &ble_param.conn_timeout);
//         //timeout = 25 * ble_param.conn_interval * BLE_CON_SLEEP_LATENCY / 20 ;
//         //ble_update_conn_param(ble_cb.con_handle, ble_param.conn_interval, BLE_CON_SLEEP_LATENCY, timeout);
//         ble_update_conn_param(ble_cb.con_handle, 400, 0, 500);
//     } else {
//         ble_get_adv_interval(&ble_param.adv_interval);
//         ble_set_adv_interval(BLE_ADV_SLEEP_INTERVAL);
//     }
// }

// static void ble_update_param_after_exit_sleep(void)
// {
//     uint16_t adv_interval = 0;
//     ble_get_adv_interval(&adv_interval);

//     if (ble_cb.con_handle) {
//         ble_disable_latency(ble_cb.con_handle);
//     }

//     if (adv_interval == BLE_ADV_SLEEP_INTERVAL) {
//         ble_set_adv_interval(ble_param.adv_interval);
//     }
// }

AT(.com_sleep.ble.sleep)
bool ble_user_service_pending(void)
{
    return ble_cb.pending;
}

AT(.text.app.proc.ble)
void ble_user_set_service_pending(void)
{
    ble_cb.pending = 1;
}

AT(.text.app.proc.ble)
void ble_user_clear_service_pending(void)
{
    ble_cb.pending = 0;
}

void ble_enter_sleep_proc(void)
{
#if ALARM_NUMBER
    ble_alarm_enter_sleep_proc();
#endif

    ble_enable_latency(ble_cb.con_handle);
    // ble_update_param_before_enter_sleep();
}

void ble_exit_sleep_proc(void)
{
#if ALARM_NUMBER
    ble_alarm_exit_sleep_proc();
#endif

    ble_disable_latency(ble_cb.con_handle);
    // ble_update_param_after_exit_sleep();
}

AT(.text.app.proc.ble)
void ble_app_proc(void)
{
    ble_adv_proc();
    ble_battery_service_proc();
    ble_hid_service_proc();
#if LE_FOTA_EN
    ble_fota_process();
#endif

    ble_user_clear_service_pending();
}

AT(.com_sleep.ble.sleep)
bool ble_proc_pending(void)
{
    bool res = false;

#if LE_FOTA_EN
    res = ble_fota_is_start();
#endif

    if(!res){
        res = ble_user_service_pending();
    }

    return res;
}
