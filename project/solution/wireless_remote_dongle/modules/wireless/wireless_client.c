#include "include.h"
#include "usb_usr_audio.h"
#include "usb_usr_hid.h"

#define TRACE_EN                            1
#define TRACE_RATE_EN                       0

#if TRACE_EN
#define TRACE(...)                          my_printf("[WIRELESS] ");\
                                            my_printf(__VA_ARGS__)
#define TRACE_R(...)                        my_print_r(__VA_ARGS__);
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#define CFG_CONN_NUM                        WIRELESS_CONN_NB

#if CFG_CONN_NUM == 1
#define CFG_CONN_INTV                       8         // Unit: 1.25ms
#define CFG_CONN_LAT                        100       // Unit: connection interval
#define CFG_CONN_TO                         300       // Unit: 10ms
#define CFG_RSSI_FILTER                     -60
#elif CFG_CONN_NUM == 2
#define CFG_CONN_INTV                       4       // Unit: 1.25ms
#define CFG_CONN_LAT                        103     // Unit: connection interval
#define CFG_CONN_TO                         200     // Unit: 10ms
#define CFG_CONN2_INTV                      16      // Unit: 1.25ms
#define CFG_CONN2_LAT                       25      // Unit: connection interval
#define CFG_CONN2_TO                        200     // Unit: 10ms
#define CFG_RSSI_FILTER                     -60
#else
#error "[WARNING] wireless param 'CFG_CONN_NUM' value not support!"
#endif

#define TAG_CONHDL_INVALID                  0xff


typedef struct {
    uint16_t        con_handle;
    #if TRACE_RATE_EN
    uint32_t        stat_poll_tick;
    uint32_t        stat_pkt_cnt;
    uint32_t        stat_byte_cnt;
    #endif
} wireless_cb_t;

static wireless_cb_t wireless_cb[CFG_CONN_NUM];

static bool wireless_init_ongoing = false;

const uint8_t test_data[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};


static wireless_cb_t *wireless_instance_alloc(uint8_t conhdl)
{
    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle == TAG_CONHDL_INVALID) {
            wireless_cb[i].con_handle = conhdl;
            return &wireless_cb[i];
        }
    }

    return NULL;
}

static bool wireless_instance_free(uint8_t conhdl)
{
    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle == conhdl) {
            wireless_cb[i].con_handle = TAG_CONHDL_INVALID;
            return true;
        }
    }

    return false;
}

static uint8_t wireless_instance_count(void)
{
    uint8_t count = 0;

    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle != TAG_CONHDL_INVALID) {
            count++;
        }
    }

    return count;
}

static wireless_cb_t *wireless_instance_query(uint8_t conhdl)
{
    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle == conhdl) {
            return &wireless_cb[i];
        }
    }

    return NULL;
}

static void wireless_event_packet_handler(uint8_t event_type, uint8_t *packet, uint8_t size)
{
    switch (event_type) {
        case WIRELESS_EVENT_CONNECTED:
            TRACE("connected - con_handle: 0x%04x\n", GET_LE16(&packet[0]));
            wireless_init_ongoing = false;
            wireless_instance_alloc(GET_LE16(&packet[0]));
            if (wireless_instance_count() < CFG_CONN_NUM) {
                wireless_scan_ctrl(true);
            }
            break;

        case WIRELESS_EVENT_DISCONNECTED:
            TRACE("disconnected - con_handle: 0x%04x  reason: 0x%02x\n", GET_LE16(&packet[0]), packet[2]);
            wireless_instance_free(GET_LE16(&packet[0]));
            wireless_scan_ctrl(true);
            break;

        case WIRELESS_EVENT_ADV_REPORT: {
            wireless_adv_report_t *adv_report_p = (wireless_adv_report_t *)&packet[0];
            uint16_t intv, lat, sup_to;

            TRACE("adv report - type: 0x%02x rssi: %d addr: ", adv_report_p->addr_type, adv_report_p->rssi);
            TRACE_R(adv_report_p->addr, 6);

            /* adv_report_p->report equ PeripheralDevice::wl_adv_data */
            TRACE("adv data[%d]: ", adv_report_p->report_len);
            TRACE_R(adv_report_p->report, adv_report_p->report_len);
            /* Maybe it can filter by adv or address */
            #if (CFG_CONN_NUM > 1)
            if (wireless_instance_count()) {
                intv   = CFG_CONN2_INTV;
                lat    = CFG_CONN2_LAT;
                sup_to = CFG_CONN2_TO;
            } else
            #endif
            {
                intv   = CFG_CONN_INTV;
                lat    = CFG_CONN_LAT;
                sup_to = CFG_CONN_TO;
            }

            if (adv_report_p->rssi >= CFG_RSSI_FILTER && !wireless_init_ongoing) {
                TRACE("connect to device: ");
                TRACE_R(adv_report_p->addr, 6);
                wireless_scan_ctrl(false);
                wireless_connect_by_addr(adv_report_p->addr, adv_report_p->addr_type, intv, lat, sup_to, 3000);  //interval 10ms
                wireless_init_ongoing = true;
            }
        } break;

        case WIRELESS_EVENT_INIT_TERMINATED: {
            wireless_init_terminated_t *param = (wireless_init_terminated_t *)packet;
            TRACE("init terminated: 0x%02x\n", param->reason);
            wireless_init_ongoing = false;
            /* It restart scan only when timeout and other reason need to be handled manually */
            if (param->reason == 0x04) {
                wireless_scan_ctrl(true);
            }
            break;
        }

        default:
            break;
    }
}

AT(.com_text.wireless.transmit)
static void wireless_receive_handler(uint16_t con_handle, uint8_t *packet, uint8_t size)
{
    wireless_cb_t *instance = wireless_instance_query(con_handle);

    if (instance != NULL) {
        // TRACE("con[%d] rx(%d):\n", instance->con_handle, size);
        // TRACE_R(packet, size);
#if USB_HID_EN
        if (packet[0] == WIRELESS_PACKET_TYPE_HID) {
            usb_usr_hid_keyboard_write((hid_rep_keyboard_t *)&packet[2]);
        }
#endif

#if USB_MIC_EN
        if (packet[0] == WIRELESS_PACKET_TYPE_AUDIO && packet[1] == AUDIO_MIC_PACKET_SIZE) {
            usb_usr_audio_mic_write(&packet[2], packet[1]);
        }
#endif
    }

    wireless_rxbuf_release(packet);
}

#if SYS_SLEEP_LEVEL

void wireless_enter_sleep_hook(void)
{
    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle != TAG_CONHDL_INVALID) {
            /* Nothing to do */
        }
    }

    if (wireless_scan_state_get()) {
        /* Nothing to do */
    }
}

void wireless_exit_sleep_hook(void)
{
    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        if (wireless_cb[i].con_handle != TAG_CONHDL_INVALID) {
            /* Nothing to do */
        }
    }

    if (wireless_scan_state_get()) {
        /* Nothing to do */
    }
}

#endif

void wireless_client_init(void)
{
    printf("%s\n", __func__);

    memset(&wireless_cb, 0x00, sizeof(wireless_cb));

    for (u8 i = 0; i < CFG_CONN_NUM; i++) {
        wireless_cb[i].con_handle = TAG_CONHDL_INVALID;
    }

    wireless_event_handler_register(wireless_event_packet_handler);
    wireless_receive_handler_register(wireless_receive_handler);

    TRACE("rssi filter: %d\n", CFG_RSSI_FILTER);
}

void wireless_client_process(void)
{

}
