#include "include.h"
#include "usb_com.h"
#include "usb_hid.h"
#include "usb_vendor.h"
#include "usb_vendor_desc.h"

#if USB_VENDOR_EN


#if USB_VENCOR_DATA_IN_TEST
uint8_t usb_vendor_test_buf[4] AT(.usb_buf.debug);
uint16_t usb_vendor_test_total = 0;
uint16_t usb_vendor_test_remain = 0;
#endif // USB_VENCOR_DATA_IN_TEST

udv_t udv_0 AT(.usb_buf.usb);

epcb_t ep_vendor_tx AT(.usb_buf.usb);
epcb_t ep_vendor_rx AT(.usb_buf.usb);

u8 ep_vendor_buf_out[VENDOR_EP_IN_MAX_SIZE + 4] AT(.usb_buf.hid);          // CRC
u8 ep_vendor_buf_in[VENDOR_EP_OUT_MAX_SIZE] AT(.usb_buf.hid);


/* Rewrite this func to notify upper layer */
void usb_vendor_receive_callback(uint8_t *buf, uint32_t size)
{
    printf("<<<vendor data out[%d]:\n", size);
    print_r(buf, size);
}

#if USB_VENCOR_DATA_IN_TEST
/* Rewrite this func to return valid data to usb core when ep ready */
void usb_vendor_send_prepare(uint8_t *buf, uint32_t *size)
{
    if (usb_vendor_test_remain) {
        *size = (usb_vendor_test_remain < VENDOR_EP_IN_MAX_SIZE)? usb_vendor_test_remain: VENDOR_EP_IN_MAX_SIZE;
        memcpy(buf, &usb_vendor_test_buf[usb_vendor_test_total - usb_vendor_test_remain], *size);
        usb_vendor_test_remain -= *size;

        printf(">>>vendor data in[%d]:\n", *size);
        print_r(buf, *size);

    } else {
        *size = 0;
    }
}
#endif // USB_VENCOR_DATA_IN_TEST

AT(.usbdev.com)
void udv_rx_process(void)
{
    u16 size;
    udv_t *udv = &udv_0;

    size = usb_ep_get_rx_len(udv->int_out);

    usb_hid_receive_callback(udv->int_out->buf, size);

    usb_ep_clear_rx_fifo(udv->int_out);
}

AT(.usbdev.com)
void udv_tx_process(void)
{
    udv_t *udv = &udv_0;
    u32 size;

    if (usb_ep_transfer(udv->int_in)) {
        usb_hid_send_prepare(udv->int_in->buf, &size);
        if (size) {
            usb_ep_start_transfer(udv->int_in, size);
        }
    }
}

void usb_vendor_send_kick(void)
{
    udv_t *udv = &udv_0;
    u32 size;

    if (usb_ep_transfer(udv->int_in)) {
        usb_vendor_send_prepare(udv->int_in->buf, &size);
        if (size) {
            usb_ep_start_transfer(udv->int_in, size);
        }
    }
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

///USB HID 初始化
void udv_init(void)
{
    epcb_t *epcb;
    udv_t *udv = &udv_0;

    udv_0.int_in = &ep_vendor_tx;
    udv_0.int_out = &ep_vendor_rx;

    memset(udv->int_in, 0x00, sizeof(epcb_t));
    memset(udv->int_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(udv_tx_process, USB_VENDOR_EP_IN_INDEX, EP_DIR_IN);
    usb_ep_callback_register(udv_rx_process, USB_VENDOR_EP_OUT_INDEX, EP_DIR_OUT);

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
}

void usb_vendor_init(void)
{
    /* Add application layer initialize if need */
}

void usb_vendor_deinit(void)
{
    /* Add application layer initialize if need */
}

#if USB_VENCOR_DATA_IN_TEST
/* Demo: kick usb core start transport */
void usb_vendor_tx_kick(void)
{
    /* Check if last frame is tx complete */
    if (usb_vendor_test_remain) {
        return;
    }

    /* App layer load data to buffer */
    PUT_LE16(&usb_vendor_test_buf[0], HID_CCC_VOLUME_DECREMENT);
    PUT_LE16(&usb_vendor_test_buf[2], HID_CCC_RELEASE);

    /* Config transport parameter */
    usb_vendor_test_total = 4;
    usb_vendor_test_remain = 4;

    /* Kick USB core to transport data */
    /* The usb core will request data by callback func "usb_vendor_send_callback" when endpoint ready */
    usb_vendor_send_kick();
}
#endif // USB_VENCOR_DATA_IN_TEST

#endif // USB_VENDOR_EN
