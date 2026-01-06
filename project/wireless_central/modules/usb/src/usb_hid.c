#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_hid_desc.h"
#include "usb_usr_hid.h"

#if USB_HID_EN

#define HID_RESUME_PREVENT_TIME             1000    // Unit: ms

#define usb_hid_resume_prevent_en()         {tick_resume_prevent = tick_get();}
#define usb_hid_resume_prevent_dis()        {tick_resume_prevent = 0;}
#define usb_hid_resume_prevent_chk()        (!tick_resume_prevent || !tick_check_expire(tick_resume_prevent, HID_RESUME_PREVENT_TIME))


udh_t udh_0 AT(.usb_buf.usb);
epcb_t ep_hid_tx AT(.usb_buf.usb);
epcb_t ep_hid_rx AT(.usb_buf.usb);

u8 ep_hid_buf_out[HID_EP_OUT_MAX_SIZE + 4] AT(.usb_buf.hid);          // CRC
u8 ep_hid_buf_in[HID_EP_IN_MAX_SIZE] AT(.usb_buf.hid);

u32 tick_resume_prevent AT(.bss.usb_buf.hid);


void ude_hid_setvalid(bool valid)
{
    udh_t *udh = &udh_0;
    udh->valid = valid;

    if (usb_usr_hid_cb.ready) {
        usb_usr_hid_cb.ready(valid);
    }
}

bool ude_hid_is_valid(void)
{
    return (udh_0.valid && !usb_device_is_suspend());
}

AT(.usbdev.com)
void ude_hid_rx_process(void)
{
    u16 size;
    udh_t *udh = &udh_0;

    size = usb_ep_get_rx_len(udh->int_out);

    if (usb_usr_hid_cb.data_out) {
        usb_usr_hid_cb.data_out(udh->int_out->buf, size);
    }

    usb_ep_clear_rx_fifo(udh->int_out);
}

AT(.usbdev.com)
void ude_hid_tx_process(void)
{
    usb_hid_in_callback();
}

AT(.com_text.isr.suspend)
void usb_hid_suspend_callback(bool suspend)
{
    if (suspend) {
        usb_hid_resume_prevent_en();

        if (usb_usr_hid_cb.suspend) {
            usb_usr_hid_cb.suspend();
        }
    } else {
        usb_hid_resume_prevent_dis();

        if (usb_usr_hid_cb.resume) {
            usb_usr_hid_cb.resume();
        }
    }
}

///USB hid endpoint reset
void usb_int_ep_reset(void)
{
    udh_t *udh = &udh_0;

    /* Any usb class can't always use ep0, so it't un-init when index is zere */

    if (udh->int_in->index) {
        usb_ep_reset(udh->int_in);
    }

    if (udh->int_out->index) {
        usb_ep_reset(udh->int_out);
    }
}

AT(.usbdev.com)
void usb_hid_sof_isr(void)
{
    if (usb_usr_hid_cb.sof) {
        usb_usr_hid_cb.sof();
    }
}

///USB HID 初始化
void udh_init(void)
{
    epcb_t *epcb;
    udh_t *udh = &udh_0;

    memset(&udh_0, 0, sizeof(udh_0));

    udh_0.int_in = &ep_hid_tx;
    udh_0.int_out = &ep_hid_rx;

    memset(udh->int_in, 0x00, sizeof(epcb_t));
    memset(udh->int_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(ude_hid_tx_process, USB_HID_EP_IN_INDEX, EP_DIR_IN);
    usb_ep_callback_register(ude_hid_rx_process, USB_HID_EP_OUT_INDEX, EP_DIR_OUT);

    memset(ep_hid_buf_in,  0, sizeof(ep_hid_buf_in));
    epcb = udh->int_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_HID_EP_IN_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_IN_MAX_SIZE;
    epcb->buf = ep_hid_buf_in;

    if (usb_ep_get_max_len(epcb->index) < HID_EP_IN_MAX_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    memset(ep_hid_buf_out,  0, sizeof(ep_hid_buf_out));
    epcb = udh->int_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_HID_EP_OUT_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_OUT_MAX_SIZE;
    epcb->buf = ep_hid_buf_out;

    if (usb_ep_get_max_len(epcb->index) < HID_EP_OUT_MAX_SIZE) {
        printf("--->err: ep size overflow\n");
        while(1);
    }

    usb_ep_init(epcb);

    ude_hid_setvalid(false);

    printf("udh_init success\n");

    if (usb_usr_hid_cb.init) {
        usb_usr_hid_cb.init();
    }
}

AT(.com_text.usb.kick)
void usb_hid_in_callback(void)
{
    udh_t *udh = &udh_0;
    u32 size = 0;

    if (!udh->valid) {
        return;
    }

    if (usb_device_is_suspend()) {
        if (false == usb_hid_resume_prevent_chk()) {
            usb_hid_resume_prevent_dis();
            usb_device_resume();
        }
        return;
    }

    if (usb_ep_transfer(udh->int_in)) {
        if (usb_usr_hid_cb.data_in) {
            size = usb_usr_hid_cb.data_in(udh->int_in->buf, HID_EP_IN_MAX_SIZE);
        }

        if (size) {
            usb_ep_start_transfer(udh->int_in, size);
        }
    }
}

void usb_hid_init(void)
{
    /* Add application layer initialize if need */
}

void usb_hid_deinit(void)
{
    ude_hid_setvalid(false);
}

#endif // USB_HID_EN
