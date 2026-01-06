#include "include.h"

uint8_t alarm_timer_5ms_scan_pool[BLE_ALARM_TIMER_LEN];
soft_timer_handle_t soft_timer_5ms_handle;


AT(.com_text.timer)
void alarm_timer_5ms_callback(ble_timer_handle_t handle)
{
    usr_tmr5ms_callback();
}

void wireless_module_deinit(void)
{
    wireless_off();
}

#if ALARM_TIMER_5MS_SCAN_EN

void soft_timer_5ms_disable(void)
{
    soft_timer_stop(soft_timer_5ms_handle);
}

void soft_timer_5ms_enable(void)
{
    soft_timer_start(soft_timer_5ms_handle);
}

#endif

void wireless_module_init(void)
{
    printf("%s: master\n", __func__);

    wireless_setup_param_t wireless_setup_param;
    memset(&wireless_setup_param, 0x00, sizeof(wireless_setup_param));
    wireless_setup_param.conn_nb_support = WIRELESS_CONN_NB;
    wireless_setup_param.conn_phy_forced = WIRELESS_PHY_FORCED;
    wireless_setup(&wireless_setup_param);

    wireless_scan_param_set(0, 100, 50);
    wireless_scan_ctrl(true);

#if ALARM_TIMER_5MS_SCAN_EN
    soft_timer_init(alarm_timer_5ms_scan_pool, sizeof(alarm_timer_5ms_scan_pool));
    soft_timer_acquire(&soft_timer_5ms_handle, 5, 1, alarm_timer_5ms_callback);
    soft_timer_start(soft_timer_5ms_handle);
#endif
}

void wireless_link_info_write(wireless_addr_info_typedef *addr_info)
{
    bsp_param_write((u8 *)addr_info, PARAM_WL_ADDR_INFO, PARAM_WL_ADDR_INFO_LEN);
    bsp_param_sync();
}

bool wireless_link_info_read(wireless_addr_info_typedef *addr_info)
{
    u8 buf[7] = {0};
    bsp_param_read((u8 *)addr_info, PARAM_WL_ADDR_INFO, PARAM_WL_ADDR_INFO_LEN);
    return (0 != memcmp(buf, (u8 *)addr_info, 7));
}
