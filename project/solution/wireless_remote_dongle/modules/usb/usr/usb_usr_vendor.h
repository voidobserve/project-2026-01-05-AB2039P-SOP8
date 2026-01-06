#ifndef __USB_USR_VENDOR_H
#define __USB_USR_VENDOR_H


typedef struct {
    void        (*init)         (void);
    void        (*suspend)      (void);
    void        (*resume)       (void);
    void        (*data_out)     (uint8_t *data, uint32_t size);
    uint32_t    (*data_in)      (uint8_t *data, uint32_t size);
    void        (*ready)        (bool ready);
    void        (*sof)          (void);
} usb_usr_vendor_cb_t;


typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    struct PACKED {
        uint8_t             report_id;
        hid_rep_consumer_t  consumer;
    } data;
} usb_vendor_rep_consumer_t;

typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    struct PACKED {
        uint8_t             report_id;
        hid_rep_app_t       app;
    } data;
} usb_vendor_rep_app_t;

typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    struct PACKED {
        uint8_t             report_id;
        hid_rep_dbg_t       dbg;
    } data;
} usb_vendor_rep_dbg_t;

typedef struct {
    list_hdr_t *            hdr;
    list_t *                owner;
    uint8_t                 length;
    uint8_t                 data[];
} usb_vendor_rep_t;

extern usb_usr_vendor_cb_t usb_usr_vendor_cb;


uint8_t usb_usr_vendor_consumer_write(hid_rep_consumer_t *consumer);
uint8_t usb_usr_vendor_app_write(uint8_t *buf, uint32_t size);
uint8_t usb_usr_vendor_dbg_write(uint8_t *buf, uint32_t size);


#endif // __USB_USR_VENDOR_H
