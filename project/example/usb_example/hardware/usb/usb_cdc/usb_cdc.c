#include "include.h"
#include "usb_com.h"
#include "usb_cdc.h"
#include "usb_cdc_desc.h"
#include "usb_usr_cdc.h"

#if USB_CDC_EN

cdc_t cdc_cb AT(.usb_buf.usb);

epcb_t ep_intr_in AT(.usb_buf.usb);
epcb_t ep_bulk_in AT(.usb_buf.usb);
epcb_t ep_bulk_out AT(.usb_buf.usb);

u8 ep_intr_buf_in[CDC_INTR_EP_IN_MAX_SIZE] AT(.usb_buf.hid);
u8 ep_bulk_buf_out[CDC_BULK_EP_OUT_MAX_SIZE + 4] AT(.usb_buf.hid);          // CRC
u8 ep_bulk_buf_in[CDC_BULK_EP_IN_MAX_SIZE] AT(.usb_buf.hid);


bool usb_cdc_get_line_coding(void)
{
    u8 buf[7] = {0x80, 0x25, 0x00, 0x00, 0x00, 0x00, 0x08};

   return usb_ep0_transfer(buf, 7);
   // return usb_ep0_start_transfer(buf, 7);
}



AT(.usbdev.com)
void usb_cdc_rx_process(void)
{
    cdc_t *cdc = &cdc_cb;

    u16 size = usb_ep_get_rx_len(cdc->bulk_out);

    if (usb_usr_cdc_cb.data_out) {
        usb_usr_cdc_cb.data_out(cdc->bulk_out->buf, size);
    }

    usb_ep_clear_rx_fifo(cdc->bulk_out);
}

AT(.usbdev.com)
void usb_cdc_tx_process(void)
{
    if (!usb_device_is_suspend()) {
        cdc_t *cdc = &cdc_cb;
        u32 size = 0;
        if (usb_ep_transfer(cdc->int_in)) {
            if (usb_usr_cdc_cb.data_in) {
                size = usb_usr_cdc_cb.data_in(cdc->int_in->buf, CDC_BULK_EP_IN_MAX_SIZE);
            }
            if (size) {
                usb_ep_start_transfer(cdc->int_in, size);
            }
        }
    }
}

///USB hid endpoint reset
AT(.usbdev.com)
void usb_cdc_ep_reset(void)
{
    printf("-->usb_cdc_ep_reset\n");

    cdc_t *cdc = &cdc_cb;

    if (cdc->int_in->index) {
        usb_ep_reset(cdc->int_in);
    }

    if (cdc->bulk_in->index) {
        usb_ep_reset(cdc->bulk_in);
    }

    if (cdc->bulk_out->index) {
        usb_ep_reset(cdc->bulk_out);
    }
}

///USB HID 初始化
void usb_cdc_init(void)
{
    printf("-->usb_cdc_init\n");

    epcb_t *epcb;
    cdc_t *cdc = &cdc_cb;

    cdc_cb.int_in = &ep_intr_in;

    cdc_cb.bulk_in = &ep_bulk_in;
    cdc_cb.bulk_out = &ep_bulk_out;

    memset(cdc->int_in, 0x00, sizeof(epcb_t));
    memset(cdc->bulk_in, 0x00, sizeof(epcb_t));
    memset(cdc->bulk_out, 0x00, sizeof(epcb_t));

    usb_ep_callback_register(usb_cdc_tx_process, USB_CDC_BULK_EP_IN_INDEX, 1);
    usb_ep_callback_register(usb_cdc_rx_process, USB_CDC_BULK_EP_OUT_INDEX, 0);

    memset(ep_intr_buf_in,  0, sizeof(ep_intr_buf_in));
    epcb = cdc->int_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_CDC_INTR_EP_INDEX;
    epcb->type = EP_TYPE_INTR;
    epcb->epsize = CDC_INTR_EP_IN_MAX_SIZE;
    epcb->buf = ep_intr_buf_in;
    usb_ep_init(epcb);

    memset(ep_bulk_buf_in,  0, sizeof(ep_bulk_buf_in));
    epcb = cdc->bulk_in;
    epcb->dir = EP_DIR_IN;
    epcb->index = USB_CDC_BULK_EP_IN_INDEX;
    epcb->type = EP_TYPE_BULK;
    epcb->epsize = CDC_BULK_EP_IN_MAX_SIZE;
    epcb->buf = ep_bulk_buf_in;
    usb_ep_init(epcb);

    memset(ep_bulk_buf_out,  0, sizeof(ep_bulk_buf_out));
    epcb = cdc->bulk_out;
    epcb->dir = EP_DIR_OUT;
    epcb->index = USB_CDC_BULK_EP_OUT_INDEX;
    epcb->type = EP_TYPE_BULK;
    epcb->epsize = CDC_BULK_EP_OUT_MAX_SIZE;
    epcb->buf = ep_bulk_buf_out;
    usb_ep_init(epcb);

}


AT(.usbdev.com)
void usb_cdc_bulk_in_callback(void)
{
    cdc_t *cdc = &cdc_cb;
    u32 size = 0;

    if (usb_ep_transfer(cdc->bulk_in)) {
        if (usb_usr_cdc_cb.data_in) {
            size = usb_usr_cdc_cb.data_in(cdc->bulk_in->buf, CDC_BULK_EP_IN_MAX_SIZE);
        }
        if (size) {
            usb_ep_start_transfer(cdc->bulk_in, size);
        }
    }
}

void usb_cdc_bulk_in_send(void)
{
    //thread_driver_user_msg_post(THREAD_DRIVER_MSG_CDC_BULK_IN);
    //thread_driver_msg_post(THREAD_DRIVER_MSG_CDC_BULK_IN);
}


void usb_cdc_process(void)
{

}

void usb_cdc_deinit(void)
{

}

#endif
