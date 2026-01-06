#ifndef _USB_VENDOR_DESC_H
#define _USB_VENDOR_DESC_H


#define USB_VENDOR_EP_IN_INDEX          3
#define USB_VENDOR_EP_OUT_INDEX         3

#define VENDOR_REPORT_DESC_SIZE         85
#define VENDOR_EP_OUT_MAX_SIZE          64
#define VENDOR_EP_IN_MAX_SIZE           64


uint8_t *usb_vendor_itf_desc_get(uint8_t *length);
uint8_t *usb_vendor_report_desc_get(uint8_t *length);

#endif
