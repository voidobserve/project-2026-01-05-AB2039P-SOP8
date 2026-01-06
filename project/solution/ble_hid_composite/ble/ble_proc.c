#include "include.h"
#include "ble_user_service.h"
#include "ble_fota_service.h"
#include "ble_hid_service.h"
#include "ble_battery_service.h"

#define BLE_ADV_SLEEP_INTERVAL    800   //500ms
#define BLE_CON_SLEEP_LATENCY     60

u8 peer_device_type = PEER_DEVICE_TYPE_ANDROID;

static struct ble_param_t {
    uint16_t adv_interval;
    uint16_t conn_interval;
    uint16_t conn_latency;
    uint16_t conn_timeout;
} ble_param;

static void ble_update_param_before_enter_sleep(void)
{
    if (ble_cb.con_handle) {
        ble_enable_latency(ble_cb.con_handle);
    } else {
        ble_get_adv_interval(&ble_param.adv_interval);
        ble_set_adv_interval(BLE_ADV_SLEEP_INTERVAL);
    }
}

static void ble_update_param_after_exit_sleep(void)
{
    uint16_t adv_interval = 0;
    ble_get_adv_interval(&adv_interval);

    if (ble_cb.con_handle) {
        ble_disable_latency(ble_cb.con_handle);
    }

    if (adv_interval == BLE_ADV_SLEEP_INTERVAL) {
        ble_set_adv_interval(ble_param.adv_interval);
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

AT(.text.ble.sleep)
void ble_set_service_pending(bool state)
{
    ble_cb.pending = state;
}

AT(.com_sleep.ble.sleep)
bool ble_get_service_pending(void)
{
    return ble_cb.pending;
}

AT(.text.app.proc.ble)
void ble_app_proc(void)
{
    ble_user_service_proc();
    ble_battery_service_proc();

#if LE_HID_EN
    ble_hid_service_proc();
#endif // LE_HID_EN

#if LE_FOTA_EN
    ble_fota_process();
#endif

    ble_set_service_pending(0);
}

AT(.com_sleep.ble.sleep)
bool ble_proc_pending(void)
{
    bool res = false;

#if LE_FOTA_EN
    res = ble_fota_is_start();
#endif

    if(!res){
        res = ble_get_service_pending();
    }

    return res;
}

void ble_hid_peer_device_type_set(PEER_DEVICE_TYPE_TYPEDEF type)
{
    peer_device_type = type;
}

PEER_DEVICE_TYPE_TYPEDEF ble_hid_peer_device_type_get(void)
{
    return peer_device_type;
}

bool ble_hid_peer_device_is_ios(void)
{
    return (peer_device_type == PEER_DEVICE_TYPE_IOS);
}
