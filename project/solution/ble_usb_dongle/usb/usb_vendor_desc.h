#ifndef _USB_VENDOR_DESC_H
#define _USB_VENDOR_DESC_H

#define USB_VENDOR_EP_IN_INDEX          1
#define USB_VENDOR_EP_OUT_INDEX         1


#define VENDOR_REPORT_DESC_SIZE         25
#define VENDOR_EP_OUT_MAX_SIZE          64      /* USB2.0 bulk and ep1 are both max 64B */
#define VENDOR_EP_IN_MAX_SIZE           2       /* USB2.0 bulk and ep1 are both max 64B */


uint8_t *usb_vendor_itf_desc_get(uint8_t *length);
uint8_t *usb_vendor_report_desc_get(uint8_t *length);

#endif
