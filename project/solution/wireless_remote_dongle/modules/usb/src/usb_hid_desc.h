#ifndef _USB_HID_DESC_H
#define _USB_HID_DESC_H

#define USB_HID_EP_IN_INDEX             1
#define USB_HID_EP_OUT_INDEX            1

#define HID_EP_IN_MAX_SIZE              6
#define HID_EP_OUT_MAX_SIZE             63  /// TODO(OPT):

#define HID_REPORT_DESC_SIZE            63          //需要和 desc_hid_report 的长度一致


uint8_t *usb_hid_itf_desc_get(uint8_t *length);
uint8_t *usb_hid_report_desc_get(uint8_t *length);

#endif
