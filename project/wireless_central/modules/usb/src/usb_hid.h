#ifndef _USB_HID_H
#define _USB_HID_H


typedef struct _udh_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
} udh_t;


void usb_hid_init(void);
void usb_hid_deinit(void);

void udh_init(void);
void usb_int_ep_reset(void);
void usb_hid_sof_isr(void);
void ude_hid_setvalid(bool valid);
bool ude_hid_is_valid(void);
void usb_hid_in_callback(void);
void usb_hid_suspend_callback(bool suspend);

#endif // _USB_HID_H
