#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_hid_desc.h"

#if USB_HID_EN

udh_t udh_0 AT(.usb_buf.usb);
epcb_t ep_hid_tx AT(.usb_buf.usb);
epcb_t ep_hid_rx AT(.usb_buf.usb);

u8 ep_hid_buf_out[HID_EP_OUT_MAX_SIZE + 4] AT(.usb_buf.hid);          // CRC
u8 ep_hid_buf_in[HID_EP_IN_MAX_SIZE] AT(.usb_buf.hid);


AT(.usbdev.com)
void ude_hid_setvalid(bool valid)
{
    udh_t *udh = &udh_0;
    udh->valid = valid;
}

AT(.usbdev.com)
void ude_hid_rx_process(void)
{
    udh_t *udh = &udh_0;

    if (udh->valid) {

        u16 size = usb_ep_get_rx_len(udh->int_out);

        usb_hid_receive_callback(udh->int_out->buf, size);

        usb_ep_clear_rx_fifo(udh->int_out);
    }
}

AT(.usbdev.com)
void ude_hid_tx_process(void)
{
    if (!usb_device_is_suspend()) {
        udh_t *udh = &udh_0;
        if (udh->valid) {
            u32 size;
            if (usb_ep_transfer(udh->int_in)) {
                usb_hid_send_prepare(udh->int_in->buf, &size);
                if (size) {
                    usb_ep_start_transfer(udh->int_in, size);
                }
            }
        }
    }
}

///USB hid endpoint reset
AT(.usbdev.com)
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

    ude_hid_setvalid(false);
}

///USB HID 初始化
void udh_init(void)
{
    epcb_t *epcb;
    udh_t *udh = &udh_0;

    udh_0.int_in = &ep_hid_tx;
    udh_0.int_out = &ep_hid_rx;

    memset(udh->int_in, 0x00, sizeof(epcb_t));
    memset(udh->int_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(ude_hid_tx_process, USB_HID_EP_IN_INDEX, 1);
    usb_ep_callback_register(ude_hid_rx_process, USB_HID_EP_OUT_INDEX, 0);

    memset(ep_hid_buf_in,  0, sizeof(ep_hid_buf_in));
    epcb = udh->int_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_HID_EP_IN_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_IN_MAX_SIZE;
    epcb->buf = ep_hid_buf_in;
    usb_ep_init(epcb);

    memset(ep_hid_buf_out,  0, sizeof(ep_hid_buf_out));
    epcb = udh->int_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_HID_EP_OUT_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = HID_EP_OUT_MAX_SIZE;
    epcb->buf = ep_hid_buf_out;
    usb_ep_init(epcb);

    ude_hid_setvalid(false);
}

/* START: API for upper layer */

void usb_hid_receive_callback(uint8_t *buf, uint32_t size)
{
     printf("hid data out[%d]:\n", size);
     print_r(buf, size);
}


AT(.usbdev.com)
void usb_hid_send_kick(void)
{
    udh_t *udh = &udh_0;
    u32 size;

    if (!udh->valid) {
        return;
    }

    if (usb_ep_transfer(udh->int_in)) {
        usb_hid_send_prepare(udh->int_in->buf, &size);
        if (size) {
            usb_ep_start_transfer(udh->int_in, size);
        }
    }
}


AT(.com_text.usb.hid)
void usb_hid_send_prepare(uint8_t *buf, uint32_t *size)
{
    static u8 state = 0;
    static hid_keyboard_report_typedef hid_keyboard_report_cb;

    switch (state) {
        case 0:
            memset((u8 *)&hid_keyboard_report_cb, 0x00, sizeof(hid_keyboard_report_typedef));
            /* Ctrl + A */
            hid_keyboard_report_cb.left_ctrl = 1;
            hid_keyboard_report_cb.key_list[0] = 0x04;
            break;

        case 1:
            /* Release */
            memset((u8 *)&hid_keyboard_report_cb, 0x00, sizeof(hid_keyboard_report_typedef));
            break;

        default:
            /* finish */
            *size = 0;
            state = 0;
            return;
    }

    memcpy(buf, &hid_keyboard_report_cb, sizeof(hid_keyboard_report_typedef));
    *size = sizeof(hid_keyboard_report_typedef);

    state = (state + 1) % 3;
}

void usb_hid_process(void)
{
    static u32 delay_cnt = 0;

    /* Change trigger condiction, by key or other */
    if (tick_check_expire(delay_cnt, 5000)) {
        delay_cnt = tick_get();
        usb_hid_send_kick();
    }
}

void usb_hid_deinit(void)
{
    ude_hid_setvalid(false);
}

#endif
