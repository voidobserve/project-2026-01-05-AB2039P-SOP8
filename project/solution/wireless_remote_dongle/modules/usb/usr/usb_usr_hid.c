#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_usr_hid.h"

#define TRACE_EN            1
#if TRACE_EN
#define TRACE(...)          my_printf("[HID] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#endif


#define USB_USR_HID_CACHE_KEYBOARD      5


static void usb_usr_hid_init(void);
static void usb_usr_hid_suspend(void);
static void usb_usr_hid_resume(void);
static void usb_usr_hid_data_out(uint8_t *data, uint32_t size);
static uint32_t usb_usr_hid_data_in(uint8_t *data, uint32_t size);

static bool usb_usr_hid_queue_push(void *hid_data);
static uint32_t usb_usr_hid_queue_pop(void *hid_data);
static void usb_usr_hid_queue_clear(void);

static list_t usb_usr_hid_queue_send;
static list_t usb_usr_hid_queue_keyboard;

static usb_hid_rep_keyboard_t usb_usr_hid_pkts_pool_keyboard[USB_USR_HID_CACHE_KEYBOARD];

usb_usr_hid_cb_t usb_usr_hid_cb = {
    /* init     */  usb_usr_hid_init,
    /* suspend  */  usb_usr_hid_suspend,
    /* resume   */  usb_usr_hid_resume,
    /* data_out */  usb_usr_hid_data_out,
    /* data_in  */  usb_usr_hid_data_in,
    /* ready    */  NULL,
    /* sof      */  NULL,
};


static void usb_usr_hid_init(void)
{
    usb_usr_hid_queue_send.first = NULL;
    usb_usr_hid_queue_send.last  = NULL;

    list_pool_init(
        &usb_usr_hid_queue_keyboard,
        usb_usr_hid_pkts_pool_keyboard,
        sizeof(usb_usr_hid_pkts_pool_keyboard[0]),
        sizeof(usb_usr_hid_pkts_pool_keyboard) / sizeof(usb_usr_hid_pkts_pool_keyboard[0])
    );
}

static void usb_usr_hid_suspend(void)
{
    TRACE("suspend\n");

    usb_usr_hid_queue_clear();
}

static void usb_usr_hid_resume(void)
{
    TRACE("resume\n");
}

static void usb_usr_hid_data_out(uint8_t *data, uint32_t size)
{
    TRACE("HID OUT[%d]: ", size);
    TRACE_R(data, size);
}

AT(.com_text.usb.hid.in)
static uint32_t usb_usr_hid_data_in(uint8_t *data, uint32_t size)
{
    uint32_t elt_size = usb_usr_hid_queue_pop(data);
    TRACE("HID IN[%d]:", size);
    TRACE_R(data, elt_size);

    #if 0
    if (elt_size > size) {
        printf("[ERR] hid pkts length no match!\n");
        while (1);
    }
    #endif

    return elt_size;
}

/////////////////////////////////////////////////////////////////////////////////////

AT(.com_text.usb.hid.queue)
static bool usb_usr_hid_queue_push(void *data)
{
    GLOBAL_INT_DISABLE();

    usb_hid_rep_keyboard_t *free = (usb_hid_rep_keyboard_t *)list_pop_front(&usb_usr_hid_queue_keyboard);
    bool status = false;

    if (free != NULL) {
        memcpy(&free->keyboard, data, sizeof(hid_rep_keyboard_t));
        list_push_back(&usb_usr_hid_queue_send, (list_hdr_t *)free);
        status = true;
    }

    GLOBAL_INT_RESTORE();

    return (status);
}

AT(.com_text.usb.hid.queue)
static uint32_t usb_usr_hid_queue_pop(void *data)
{
    GLOBAL_INT_DISABLE();

    usb_hid_rep_keyboard_t *elt = (usb_hid_rep_keyboard_t *)list_pop_front(&usb_usr_hid_queue_send);
    uint32_t result = 0;

    if (elt != NULL) {
        if (data != NULL) {
            memcpy(data, &elt->keyboard, sizeof(hid_rep_keyboard_t));
        }
        list_push_back(&usb_usr_hid_queue_keyboard, (list_hdr_t *)elt);
        result = sizeof(hid_rep_keyboard_t);
    }

    GLOBAL_INT_RESTORE();

    return (result);
}

AT(.com_text.usb.hid.queue)
static void usb_usr_hid_queue_clear(void)
{
    GLOBAL_INT_DISABLE();
    while (usb_usr_hid_queue_pop(NULL));
    GLOBAL_INT_RESTORE();
}

/////////////////////////////////////////////////////////////////////////////////////

AT(.com_text.usb.hid.data.keyboard)
uint8_t usb_usr_hid_keyboard_write(hid_rep_keyboard_t *keyboard)
{
    if (!ude_cb.connected) {
        return USB_ERR_NO_CONN;
    }

    if (!ude_hid_is_valid()) {
        return USB_ERR_BUSY;
    }

    if (!usb_usr_hid_queue_push(keyboard)) {
        return USB_ERR_NO_CAP;
    }

    thread_driver_user_msg_post(THREAD_DRIVER_MSG_HID_IN);

    return USB_ERR_OK;
}
