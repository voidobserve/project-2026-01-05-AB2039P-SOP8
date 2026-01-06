#ifndef _USB_HID_H
#define _USB_HID_H

typedef struct _udh_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
} udh_t;


/* Note: HID each packet length should be according with HID_EP_IN_MAX_SIZE */
typedef struct PACKED {
    uint8_t btn_left    : 1;
    uint8_t btn_right   : 1;
    uint8_t btn_central : 1;
    uint8_t reserved    : 5;
    int8_t  wheel;
    int8_t  x;
    int8_t  y;
} hid_report_info_typedef;


void usb_hid_init(void);
void usb_hid_deinit(void);
uint32_t usb_hid_buffer_push(uint8_t *buf, uint32_t size);
void usb_hid_enable(bool sta);
void usb_hid_send_test(void);
void udh_init(void);
void usb_int_ep_reset(void);
void ude_hid_setvalid(bool valid);
void usb_hid_send_prepare(uint8_t *buf, uint32_t *size);
void usb_hid_in_callback(void);

#endif // _USB_HID_H
