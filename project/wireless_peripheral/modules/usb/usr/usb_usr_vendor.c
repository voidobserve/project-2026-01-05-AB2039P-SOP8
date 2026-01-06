#include "include.h"
#include "usb_com.h"
#include "usb_vendor.h"
#include "usb_usr_vendor.h"
#include "fota_proc.h"

#define TRACE_EN            1
#if TRACE_EN
#define TRACE(...)          my_printf("[VENDOR] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#endif

#define USB_USB_VENDOR_DBG_EN                       0

#define USB_USR_VENDOR_CACHE_CONSUMER               5
#define USB_USR_VENDOR_CACHE_APP                    2
#define USB_USR_VENDOR_CACHE_DBG                    2


static void usb_usr_vendor_init(void);
static void usb_usr_vendor_data_out(uint8_t *data, uint32_t size);
static uint32_t usb_usr_vendor_data_in(uint8_t *data, uint32_t size);
static void usb_usr_vendor_ready(bool ready);
#if USB_USB_VENDOR_DBG_EN
static void usb_usr_vendor_sof(void);
#endif

static uint8_t usb_usr_vendor_queue_push(void *data, uint32_t length, list_t *owner, uint8_t report_id);
static uint32_t usb_usr_vendor_queue_pop(void *packet);
static void usb_usr_vendor_queue_clear(void);
#if USB_USB_VENDOR_DBG_EN
static void usb_usr_vendor_dbg_putchar(char ch);
#endif
#if AB_FOT_CHL_USB_EN
static int usb_usr_vendor_app_send(uint8_t *buf, uint16_t size);
static int usb_usr_vendor_app_max_pkt_get(uint32_t *size);
#endif

static list_t usb_usr_vendor_queue_send;
static list_t usb_usr_vendor_queue_consumer;
static list_t usb_usr_vendor_queue_app;
static list_t usb_usr_vendor_queue_dbg;

static usb_vendor_rep_consumer_t usb_usr_vendor_pkts_pool_consumer[USB_USR_VENDOR_CACHE_CONSUMER];
static usb_vendor_rep_app_t usb_usr_vendor_pkts_pool_app[USB_USR_VENDOR_CACHE_APP];
static usb_vendor_rep_dbg_t usb_usr_vendor_pkts_pool_dbg[USB_USR_VENDOR_CACHE_DBG];

#if USB_USB_VENDOR_DBG_EN
static usb_vendor_rep_dbg_t *usb_usr_vendor_dbg_curr;
#endif

usb_usr_vendor_cb_t usb_usr_vendor_cb = {
    /* init     */  usb_usr_vendor_init,
    /* suspend  */  NULL,
    /* resume   */  NULL,
    /* data_out */  usb_usr_vendor_data_out,
    /* data_in  */  usb_usr_vendor_data_in,
    /* ready    */  usb_usr_vendor_ready,
    #if USB_USB_VENDOR_DBG_EN
    /* sof      */  usb_usr_vendor_sof,
    #else
    /* sof      */  NULL,
    #endif
};


static void usb_usr_vendor_init(void)
{
    usb_usr_vendor_queue_send.first = NULL;
    usb_usr_vendor_queue_send.last  = NULL;

    #if USB_USB_VENDOR_DBG_EN
    usb_usr_vendor_dbg_curr = NULL;
    #endif
    
    list_pool_init(
        &usb_usr_vendor_queue_consumer,
        usb_usr_vendor_pkts_pool_consumer,
        sizeof(usb_usr_vendor_pkts_pool_consumer[0]),
        sizeof(usb_usr_vendor_pkts_pool_consumer) / sizeof(usb_usr_vendor_pkts_pool_consumer[0])
    );

    list_pool_init(
        &usb_usr_vendor_queue_app,
        usb_usr_vendor_pkts_pool_app,
        sizeof(usb_usr_vendor_pkts_pool_app[0]),
        sizeof(usb_usr_vendor_pkts_pool_app) / sizeof(usb_usr_vendor_pkts_pool_app[0])
    );

    list_pool_init(
        &usb_usr_vendor_queue_dbg,
        usb_usr_vendor_pkts_pool_dbg,
        sizeof(usb_usr_vendor_pkts_pool_dbg[0]),
        sizeof(usb_usr_vendor_pkts_pool_dbg) / sizeof(usb_usr_vendor_pkts_pool_dbg[0])
    );

#if AB_FOT_CHL_USB_EN
    ab_fota_cb_t param;
    param.fota_send_packet_handle = usb_usr_vendor_app_send;
    param.fota_mtu_check_handle = usb_usr_vendor_app_max_pkt_get;
    fota_init(&param, FOTA_CHL_USB);
#endif
}

static void usb_usr_vendor_data_out(uint8_t *data, uint32_t size)
{
    TRACE("data out[%d]:\n", size);
    TRACE_R(data, size);

#if AB_FOT_CHL_USB_EN
    if (data[0] == HID_REP_ID_APP) {
        hid_rep_app_t *pkt = (hid_rep_app_t *)&data[1];
        fota_recv_packet_handle(pkt->data, pkt->length, FOTA_CHL_USB);
    }
#endif
}

static uint32_t usb_usr_vendor_data_in(uint8_t *data, uint32_t size)
{
    uint32_t elt_size = usb_usr_vendor_queue_pop(data);

    if (elt_size) {
        #if USB_USB_VENDOR_DBG_EN
        if (data[0] != HID_REP_ID_DGB)
        #endif
        {
            TRACE("data in[%d]:\n", elt_size);
            TRACE_R(data, elt_size);
        }
    }

    #if 0
    if (elt_size > size) {
        printf("[ERR] vendor pkts length no match!\n");
        while (1);
    }
    #endif

    return elt_size;
}

static void usb_usr_vendor_ready(bool ready)
{
    if (ready) {
        usb_usr_vendor_queue_clear();
        #if USB_USB_VENDOR_DBG_EN
        my_printf_init(usb_usr_vendor_dbg_putchar);
        #endif
    } else {
        #if USB_USB_VENDOR_DBG_EN
        #if BSP_UART_DEBUG_EN
        my_printf_init(NULL);
        #else
        void uart_debug_putchar(char ch);
        my_printf_init(uart_debug_putchar);
        #endif
        #endif
    }
}

#if USB_USB_VENDOR_DBG_EN

AT(.com_text.usb.vendor.usr)
static void usb_usr_vendor_sof(void)
{
    /* Interrupt function */

    if (usb_usr_vendor_dbg_curr) {
        if (usb_usr_vendor_dbg_curr->length) {
            usb_usr_vendor_dbg_curr->owner = &usb_usr_vendor_queue_dbg;
            memset(&usb_usr_vendor_dbg_curr->data.dbg.data[usb_usr_vendor_dbg_curr->length], 0x00, sizeof(hid_rep_dbg_t) - usb_usr_vendor_dbg_curr->length);
            usb_usr_vendor_dbg_curr->length = sizeof(hid_rep_dbg_t) + 1;
            usb_usr_vendor_dbg_curr->data.report_id = HID_REP_ID_DGB;
            list_push_back(&usb_usr_vendor_queue_send, (list_hdr_t *)usb_usr_vendor_dbg_curr);
            thread_driver_user_msg_post(THREAD_DRIVER_MSG_VENDOR_IN);
            usb_usr_vendor_dbg_curr = NULL;
        }
    }

    if (usb_usr_vendor_dbg_curr == NULL) {
        usb_usr_vendor_dbg_curr = (usb_vendor_rep_dbg_t *)list_pop_front(&usb_usr_vendor_queue_dbg);
        if (usb_usr_vendor_dbg_curr) {
            usb_usr_vendor_dbg_curr->length = 0;
        }
    }
}

#endif // USB_USB_VENDOR_DBG_EN

/////////////////////////////////////////////////////////////////////////////////////

AT(.com_text.usb.vendor.queue)
static uint8_t usb_usr_vendor_queue_push(void *data, uint32_t length, list_t *owner, uint8_t report_id)
{
    if (!ude_cb.connected) {
        return USB_ERR_NO_CONN;
    }

    if (!ude_vendor_is_valid()) {
        return USB_ERR_BUSY;
    }

    GLOBAL_INT_DISABLE();

    usb_vendor_rep_t *free = (usb_vendor_rep_t *)list_pop_front(owner);
    uint8_t status = USB_ERR_NO_CAP;

    if (free != NULL) {
        free->owner   = owner;
        free->length  = length + 1;
        free->data[0] = report_id;
        memcpy(&free->data[1], data, length);
        list_push_back(&usb_usr_vendor_queue_send, (list_hdr_t *)free);
        status = USB_ERR_OK;

        thread_driver_user_msg_post(THREAD_DRIVER_MSG_VENDOR_IN);
    }

    GLOBAL_INT_RESTORE();

    return status;
}

AT(.com_text.usb.vendor.queue)
static uint32_t usb_usr_vendor_queue_pop(void *packet)
{
    GLOBAL_INT_DISABLE();

    usb_vendor_rep_t *elt = (usb_vendor_rep_t *)list_pop_front(&usb_usr_vendor_queue_send);
    uint32_t result = 0;

    if (elt != NULL) {
        if (packet != NULL) {
            memcpy(packet, &elt->data, elt->length);
        }
        result = elt->length;
        list_push_back(elt->owner, (list_hdr_t *)elt);
    }

    GLOBAL_INT_RESTORE();

    return (result);
}

AT(.com_text.usb.vendor.queue)
static void usb_usr_vendor_queue_clear(void)
{
    GLOBAL_INT_DISABLE();
    while (usb_usr_vendor_queue_pop(NULL));
    GLOBAL_INT_RESTORE();
}

#if USB_USB_VENDOR_DBG_EN

AT(.com_text.usb.vendor.usr)
static void usb_usr_vendor_dbg_putchar(char ch)
{
    GLOBAL_INT_DISABLE();
    if (usb_usr_vendor_dbg_curr && usb_usr_vendor_dbg_curr->length < sizeof(hid_rep_dbg_t)) {
        usb_usr_vendor_dbg_curr->data.dbg.data[usb_usr_vendor_dbg_curr->length++] = ch;
    }
    GLOBAL_INT_RESTORE();
}

#endif // USB_USB_VENDOR_DBG_EN

#if AB_FOT_CHL_USB_EN

static int usb_usr_vendor_app_send(uint8_t *buf, uint16_t size)
{
    return (USB_ERR_OK == usb_usr_vendor_app_write(buf, size))? 0: -1;
}

static int usb_usr_vendor_app_max_pkt_get(uint32_t *size)
{
    *size = sizeof(hid_rep_app_t) - offsetof(hid_rep_app_t, data);

    return 0/* No error */;
}

#endif // AB_FOT_CHL_USB_EN

/////////////////////////////////////////////////////////////////////////////////////

uint8_t usb_usr_vendor_consumer_write(hid_rep_consumer_t *consumer)
{
    return usb_usr_vendor_queue_push(consumer, sizeof(hid_rep_consumer_t), &usb_usr_vendor_queue_consumer, HID_REP_ID_CONSUMER);
}

uint8_t usb_usr_vendor_app_write(uint8_t *buf, uint32_t size)
{
    hid_rep_app_t pdu;

    if (size > sizeof(pdu)) {
        return USB_ERR_PKT_TOO_LONG;
    }

    pdu.length = size;
    memcpy(pdu.data, buf, size);
    memset(pdu.data + size, 0x00, sizeof(pdu.data) - size);

    return usb_usr_vendor_queue_push(&pdu, sizeof(pdu), &usb_usr_vendor_queue_app, HID_REP_ID_APP);
}

uint8_t usb_usr_vendor_dbg_write(uint8_t *buf, uint32_t size)
{
    hid_rep_dbg_t pdu;

    if (size > sizeof(pdu)) {
        return USB_ERR_PKT_TOO_LONG;
    }

    memcpy(pdu.data, buf, size);
    memset(pdu.data + size, 0x00, sizeof(pdu) - size);

    return usb_usr_vendor_queue_push(&pdu, sizeof(pdu), &usb_usr_vendor_queue_dbg, HID_REP_ID_DGB);
}
