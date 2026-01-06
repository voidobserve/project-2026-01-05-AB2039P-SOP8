#ifndef _USB_VENDOR_H
#define _USB_VENDOR_H

typedef struct _udv_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
} udv_t;


void usb_vendor_init(void);
void usb_vendor_deinit(void);

void udv_init(void);
void usb_vendor_ep_reset(void);
void usb_vendor_sof_isr(void);
void ude_vendor_setvalid(bool valid);
bool ude_vendor_is_valid(void);
void usb_vendor_send_kick(void);

#endif // _USB_VENDOR_H
