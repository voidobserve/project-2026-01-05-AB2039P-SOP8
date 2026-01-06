#include "include.h"
#include "usb_com.h"
#include "usb_vendor.h"
#include "usb_vendor_desc.h"
#include "usb_usr_vendor.h"
#include "fota_proc.h"

#if USB_VENDOR_EN

udv_t udv_0 AT(.usb_buf.usb);
epcb_t ep_vendor_tx AT(.usb_buf.usb);
epcb_t ep_vendor_rx AT(.usb_buf.usb);

u8 ep_vendor_buf_out[VENDOR_EP_OUT_MAX_SIZE + 4] AT(.usb_buf.vendor);          // CRC
u8 ep_vendor_buf_in[VENDOR_EP_IN_MAX_SIZE] AT(.usb_buf.vendor);


void ude_vendor_setvalid(bool valid)
{
    udv_t *udv = &udv_0;
    udv->valid = valid;
    
    if (usb_usr_vendor_cb.ready) {
        usb_usr_vendor_cb.ready(valid);
    }
}

bool ude_vendor_is_valid(void)
{
    return (udv_0.valid && !usb_device_is_suspend());
}

AT(.usbdev.com)
void ude_vendor_rx_process(void)
{
    u16 size;
    udv_t *udv = &udv_0;

    size = usb_ep_get_rx_len(udv->int_out);

    if (usb_usr_vendor_cb.data_out) {
        usb_usr_vendor_cb.data_out(udv->int_out->buf, size);
    }

    usb_ep_clear_rx_fifo(udv->int_out);
}

AT(.usbdev.com)
void ude_vendor_tx_process(void)
{
    usb_vendor_send_kick();
}

///USB hid endpoint reset
void usb_vendor_ep_reset(void)
{
    udv_t *udv = &udv_0;

    /* Any usb class can't always use ep0, so it't un-init when index is zere */

    if (udv->int_in->index) {
        usb_ep_reset(udv->int_in);
    }

    if (udv->int_out->index) {
        usb_ep_reset(udv->int_out);
    }
}

AT(.usbdev.com)
void usb_vendor_sof_isr(void)
{
    if (usb_usr_vendor_cb.sof) {
        usb_usr_vendor_cb.sof();
    }
}

///USB HID 初始化
void udv_init(void)
{
    epcb_t *epcb;
    udv_t *udv = &udv_0;

    udv_0.int_in = &ep_vendor_tx;
    udv_0.int_out = &ep_vendor_rx;

    memset(udv->int_in, 0x00, sizeof(epcb_t));
    memset(udv->int_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(ude_vendor_tx_process, USB_VENDOR_EP_IN_INDEX, EP_DIR_IN);
    usb_ep_callback_register(ude_vendor_rx_process, USB_VENDOR_EP_OUT_INDEX, EP_DIR_OUT);

    memset(ep_vendor_buf_in,  0, sizeof(ep_vendor_buf_in));
    epcb = udv->int_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_VENDOR_EP_IN_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = VENDOR_EP_IN_MAX_SIZE;
    epcb->buf = ep_vendor_buf_in;

    if (usb_ep_get_max_len(epcb->index) < VENDOR_EP_IN_MAX_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    memset(ep_vendor_buf_out,  0, sizeof(ep_vendor_buf_out));
    epcb = udv->int_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_VENDOR_EP_OUT_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = VENDOR_EP_OUT_MAX_SIZE;
    epcb->buf = ep_vendor_buf_out;

    if (usb_ep_get_max_len(epcb->index) < VENDOR_EP_OUT_MAX_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    ude_vendor_setvalid(false);

    printf("uda_init success\n");

    if (usb_usr_vendor_cb.init) {
        usb_usr_vendor_cb.init();
    }
}

void usb_vendor_send_kick(void)
{
    udv_t *udv = &udv_0;
    u32 size = 0;

    if (!ude_vendor_is_valid()) {
        return;
    }

    if (usb_ep_transfer(udv->int_in)) {
        if (usb_usr_vendor_cb.data_in) {
            size = usb_usr_vendor_cb.data_in(udv->int_in->buf, VENDOR_EP_IN_MAX_SIZE);
        }

        if (size) {
            usb_ep_start_transfer(udv->int_in, size);
        }
    }
}

void usb_vendor_init(void)
{
    /* Add application layer initialize if need */
}

void usb_vendor_deinit(void)
{
    ude_vendor_setvalid(false);

#if AB_FOT_CHL_USB_EN
    fota_disconnected_handle(FOTA_CHL_USB);
#endif
}

#endif // USB_VENDOR_EN
