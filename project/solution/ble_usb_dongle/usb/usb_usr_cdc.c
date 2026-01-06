#include "include.h"
#include "usb_cdc.h"
#include "usb_usr_cdc.h"

#define TRACE_EN            1
#if TRACE_EN
#define TRACE(...)          my_printf("[HID] "); \
                            my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#endif

static void usb_usr_cdc_data_out(uint8_t *data, uint32_t size);
static uint32_t usb_usr_cdc_data_in(uint8_t *data, uint32_t size);

usb_usr_cdc_cb_t usb_usr_cdc_cb = {
    NULL,
    usb_usr_cdc_data_out,
    usb_usr_cdc_data_in,
};

static void usb_usr_cdc_data_out(uint8_t *data, uint32_t size)
{
     printf("cdc data out[%d]:\n", size);
     print_r(data, size);
}

AT(.com_text.usb.hid.in)
static uint32_t usb_usr_cdc_data_in(uint8_t *data, uint32_t size)
{
    u8 test_data[] = {0x31, 0x32, 0x33};

    memcpy(data, test_data, sizeof(test_data));

    return sizeof(test_data);
}
