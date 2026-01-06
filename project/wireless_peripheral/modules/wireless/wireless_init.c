#include "include.h"

#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif


uint8_t alarm_timer_5ms_scan_pool[BLE_ALARM_TIMER_LEN];
soft_timer_handle_t soft_timer_5ms_handle;

const uint8_t wl_adv_data[] = {
    0x03, 0xff, 0x42, 0x06,
};


AT(.com_text.timer)
void alarm_timer_5ms_callback(ble_timer_handle_t handle)
{
    usr_tmr5ms_callback();
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

void wireless_link_info_clear(void)
{
    u8 buf[PARAM_WL_ADDR_INFO_LEN] = {0};
    bsp_param_write(buf, PARAM_WL_ADDR_INFO, PARAM_WL_ADDR_INFO_LEN);
    bsp_param_sync();
}

void wireless_adv_init(bool adv_en)
{
    wireless_addr_info_typedef wireless_addr_info;
    wireless_adv_param_t wireless_adv_param;

    /* ASSERT: adv data len (e.g. wl_adv_data buf size) little than or equal 31B */

    memset(&wireless_adv_param, 0x00, sizeof(wireless_adv_param));
    memcpy(&wireless_adv_param.data_buf, wl_adv_data, sizeof(wl_adv_data));
    wireless_adv_param.data_len     = sizeof(wl_adv_data);
    wireless_adv_param.intv         = 0x30;
    wireless_adv_param.channel_map  = ADV_ALL_CHNLS_EN;

    if (wireless_link_info_read(&wireless_addr_info)) {
        TRACE("config direct adv - addr: ");
        TRACE_R(wireless_addr_info.addr, 6);
        wireless_adv_param.adv_type       = ADV_TYPE_DIRECT_LO_IND;
        wireless_adv_param.peer_addr_type = wireless_addr_info.address_type;
        memcpy(wireless_adv_param.peer_addr, wireless_addr_info.addr, 6);
    } else {
        TRACE("config undirect adv\n");
        wireless_adv_param.adv_type = ADV_TYPE_IND;
    }

    wireless_adv_param_set(&wireless_adv_param);

    if (adv_en) {
        TRACE("adv enable\n");
        wireless_adv_ctrl(true);
    }
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
    printf("%s: slave\n", __func__);

    wireless_setup_param_t wireless_setup_param;
    memset(&wireless_setup_param, 0x00, sizeof(wireless_setup_param));
    wireless_setup_param.conn_nb_support = WIRELESS_CONN_NB;
    wireless_setup_param.conn_phy_forced = WIRELESS_PHY_FORCED;
    wireless_setup(&wireless_setup_param);

    wireless_adv_init(true);

#if ALARM_TIMER_5MS_SCAN_EN
    soft_timer_init(alarm_timer_5ms_scan_pool, sizeof(alarm_timer_5ms_scan_pool));
    soft_timer_acquire(&soft_timer_5ms_handle, 5, 1, alarm_timer_5ms_callback);
    soft_timer_start(soft_timer_5ms_handle);
#endif
}

void wireless_module_deinit(void)
{
    wireless_off();
}
