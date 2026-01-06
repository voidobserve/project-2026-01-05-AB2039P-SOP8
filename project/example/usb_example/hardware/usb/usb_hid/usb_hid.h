#ifndef _USB_HID_H
#define _USB_HID_H

typedef struct _udh_t {
    epcb_t *int_in;
    epcb_t *int_out;
    bool    valid;
} udh_t;


typedef struct {
    uint8_t left_ctrl   : 1;
    uint8_t left_shift  : 1;
    uint8_t left_alt    : 1;
    uint8_t left_gui    : 1;
    uint8_t right_ctrl  : 1;
    uint8_t right_shift : 1;
    uint8_t right_alt   : 1;
    uint8_t right_gui   : 1;
    uint8_t reserve;
    uint8_t key_list[6];
} hid_keyboard_report_typedef;

void usb_hid_deinit(void);
void udh_init(void);
void usb_int_ep_reset(void);
void ude_hid_setvalid(bool valid);
void usb_hid_receive_callback(uint8_t *buf, uint32_t size);
void usb_hid_send_prepare(uint8_t *buf, uint32_t *size);
void usb_hid_send_kick(void);
void usb_hid_process(void);

#endif // _USB_HID_H
