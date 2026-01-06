#ifndef _USB_CDC_H
#define _USB_CDC_H

typedef struct _cdc_t {
    epcb_t *int_in;
    epcb_t *bulk_in;
    epcb_t *bulk_out;
} cdc_t;


void usb_cdc_deinit(void);
void usb_cdc_init(void);
void usb_cdc_ep_reset(void);
void usb_cdc_receive_callback(uint8_t *buf, uint32_t size);
void usb_cdc_send_prepare(uint8_t *buf, uint32_t *size);
void usb_cdc_bulk_in_callback(void);
void usb_cdc_process(void);
bool usb_cdc_get_line_coding(void);

#endif
