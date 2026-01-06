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

static uint8_t hid_cmd_buffer[HID_EP_IN_MAX_SIZE * 10] AT(.usb_buf.hid);
static ring_buf_t hid_cmd_buffer_cb AT(.usb_buf.hid);

#if USB_HID_REPORT_RATE_TEST
static uint8_t usb_hid_test_en = 0;
static hid_report_info_typedef hid_report_info;
static u8 dir_cnt = 0;
static s8 step = +5;
#endif // USB_HID_REPORT_RATE_TEST

void ude_hid_setvalid(bool valid)
{
    udh_t *udh = &udh_0;
    udh->valid = valid;
}

void usb_hid_receive_callback(uint8_t *buf, uint32_t size)
{
     printf("hid data out[%d]:\n", size);
     print_r(buf, size);
}

AT(.usbdev.com)
void ude_hid_rx_process(void)
{
    u16 size;
    udh_t *udh = &udh_0;

    if (udh->valid) {
        size = usb_ep_get_rx_len(udh->int_out);

        usb_hid_receive_callback(udh->int_out->buf, size);

        usb_ep_clear_rx_fifo(udh->int_out);
    }
}

AT(.usbdev.com)
void ude_hid_tx_process(void)
{
    if (!usb_device_is_suspend()) {
        udh_t *udh = &udh_0;
        u32 size;

        if (udh->valid) {
            if (usb_ep_transfer(udh->int_in)) {
                usb_hid_send_prepare(udh->int_in->buf, &size);
                //print_r(udh->int_in->buf, size);
                if (size) {
                    usb_ep_start_transfer(udh->int_in, size);
                }
            }
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

    ude_hid_setvalid(false);
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
}


void usb_hid_in_callback(void)
{
    udh_t *udh = &udh_0;
    u32 size;

    // TODO: flush previous error transport buffer data???

    if (!udh->valid) {
        return;
    }

    if (usb_device_is_suspend()) {
        return;
    }

    if (usb_ep_transfer(udh->int_in)) {
        usb_hid_send_prepare(udh->int_in->buf, &size);
        if (size) {
            usb_ep_start_transfer(udh->int_in, size);
        }
    }
}

uint32_t usb_hid_buffer_push(uint8_t *buf, uint32_t size)
{
    return ring_buf_write(&hid_cmd_buffer_cb, buf, size);
}

AT(.com_text.usb.hid)
void usb_hid_send_prepare(uint8_t *buf, uint32_t *size)
{
#if USB_HID_REPORT_RATE_TEST
    if (sys_cb.usb_is_active && usb_hid_test_en) {
        hid_report_info.x = (dir_cnt % 2)? -1: 1;
        hid_report_info.y = step;
        dir_cnt++;
        if (dir_cnt >= 160) {
            dir_cnt = 0;
            step = -step;
        }
        memcpy(buf, &hid_report_info, HID_EP_IN_MAX_SIZE);
        *size = HID_EP_IN_MAX_SIZE;
        return;
    }
#endif // USB_HID_REPORT_RATE_TEST

    if (sys_cb.usb_is_active) {
        *size = ring_buf_read(&hid_cmd_buffer_cb, buf, HID_EP_IN_MAX_SIZE);
    } else {
        *size = 0;
    }
}

void usb_hid_init(void)
{
    ring_buf_init(&hid_cmd_buffer_cb, hid_cmd_buffer, sizeof(hid_cmd_buffer));
}

void usb_hid_deinit(void)
{
    ude_hid_setvalid(false);
}

#if USB_HID_REPORT_RATE_TEST
void usb_hid_enable(bool sta)
{
    if (sys_cb.usb_is_active && sta) {
        printf("start usb hid test\n");
        usb_hid_test_en = 1;
        thread_driver_user_msg_post(THREAD_DRIVER_MSG_HID_IN);
    } else {
        printf("stop usb hid test\n");
        usb_hid_test_en = 0;
    }
}
#endif // USB_HID_REPORT_RATE_TEST

void usb_hid_send_test(void)
{
    uint8_t buf[] = {0x00, 0x00, 0x00, 0x00};
    usb_hid_buffer_push(buf, 4);  //mouse 4byte
    thread_driver_user_msg_post(THREAD_DRIVER_MSG_HID_IN);
}

#endif // USB_HID_EN
