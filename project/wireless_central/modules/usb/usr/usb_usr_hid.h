#ifndef __USB_USR_HID_H
#define __USB_USR_HID_H


#define USB_USR_HID_CACHE_MOUSE                     5


typedef struct {
    void        (*init)         (void);
    void        (*suspend)      (void);
    void        (*resume)       (void);
    void        (*data_out)     (uint8_t *data, uint32_t size);
    uint32_t    (*data_in)      (uint8_t *data, uint32_t size);
    void        (*ready)        (bool ready);
    void        (*sof)          (void);
} usb_usr_hid_cb_t;

typedef struct {
    list_hdr_t *            hdr;
    hid_rep_mouse_t         mouse;
} usb_hid_rep_mouse_t;

extern usb_usr_hid_cb_t usb_usr_hid_cb;


uint8_t usb_usr_hid_mouse_write(hid_rep_mouse_t *mouse);

#endif // __USB_USR_HID_H
