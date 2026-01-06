#include "include.h"


#define TRACE_EN                            1
#define TRACE_RATE_EN                       1

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


static struct {
    uint16_t        con_handle;
    wireless_addr_info_typedef  addr_info;
    #if TRACE_RATE_EN
    uint32_t        stat_poll_tick;
    uint32_t        stat_pkt_cnt;
    #endif
} wireless_cb;


static void wireless_event_packet_handler(uint8_t event_type, uint8_t *packet, uint8_t size)
{
    switch (event_type) {
        case WIRELESS_EVENT_CONNECTED:
            TRACE("connected - con_handle: 0x%04x\n", GET_LE16(&packet[0]));
            wireless_cb.con_handle = GET_LE16(&packet[0]);
            memcpy((u8 *)&wireless_cb.addr_info, &packet[2], 7);
            wireless_link_info_write(&wireless_cb.addr_info);
            break;

        case WIRELESS_EVENT_DISCONNECTED:
            TRACE("disconnected - con_handle: 0x%04x  reason: 0x%02x\n", GET_LE16(&packet[0]), packet[2]);
            wireless_cb.con_handle = 0;
            break;

        default:
            break;
    }
}

AT(.com_text.wireless.transmit)
static void wireless_receive_handler(uint16_t con_handle, uint8_t *packet, uint8_t size)
{
    if (con_handle == wireless_cb.con_handle) {
        TRACE("rx(%d):\n", size);
        TRACE_R(packet, size);

        #if TRACE_RATE_EN
        wireless_cb.stat_pkt_cnt++;
        #endif
    }

    wireless_rxbuf_release(packet);
}

#if SYS_SLEEP_EN

void wireless_enter_sleep_hook(void)
{
    #if ALARM_TIMER_5MS_SCAN_EN
    soft_timer_5ms_disable();
    #endif

    if (wireless_cb.con_handle) {
        wireless_latency_applied(wireless_cb.con_handle, true);
    }

    if (wireless_adv_state_get()) {
        /* Nothing to do */
    }
}

void wireless_exit_sleep_hook(void)
{
    #if ALARM_TIMER_5MS_SCAN_EN
    soft_timer_5ms_enable();
    #endif

    if (wireless_cb.con_handle) {
        wireless_latency_applied(wireless_cb.con_handle, false);
    }

    if (wireless_adv_state_get()) {
        /* Nothing to do */
    }
}

#endif

void wireless_service_init(void)
{
    printf("%s\n", __func__);

    memset(&wireless_cb, 0x00, sizeof(wireless_cb));

    wireless_event_handler_register(wireless_event_packet_handler);
    wireless_receive_handler_register(wireless_receive_handler);
}

void wireless_service_process(void)
{
    #if 0 /* TEST CODE */
    {
        static uint32_t test_tick;
        if (tick_check_expire(test_tick, 1000)) {
            test_tick = tick_get();
            if (wireless_cb.con_handle) {
                wireless_send_for_con(wireless_cb.con_handle, (u8 *)0x10000, 37);
            }
        }
    }
    #endif

    #if TRACE_RATE_EN
    {
        if (tick_check_expire(wireless_cb.stat_poll_tick, 1000)) {
            wireless_cb.stat_poll_tick = tick_get();
            TRACE("rate: %dpkts/s\n", wireless_cb.stat_pkt_cnt);
            wireless_cb.stat_pkt_cnt = 0;
        }
    }
    #endif
}
