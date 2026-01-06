#ifndef _USB_VENDOR_H
#define _USB_VENDOR_H

typedef struct _udv_t {
    epcb_t *int_in;
    epcb_t *int_out;
} udv_t;

void usb_vendor_init(void);
void usb_vendor_deinit(void);
void usb_vendor_tx_kick(void);

void usb_vendor_ep_reset(void);
void udv_init(void);

#endif // _USB_VENDOR_H
