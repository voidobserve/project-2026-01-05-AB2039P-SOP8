#ifndef _USB_CDC_DESC_H
#define _USB_CDC_DESC_H

#define USB_CDC_INTR_EP_INDEX           1
#define CDC_INTR_EP_IN_MAX_SIZE         8

#define USB_CDC_BULK_EP_IN_INDEX        2
#define CDC_BULK_EP_IN_MAX_SIZE         64

#define USB_CDC_BULK_EP_OUT_INDEX       2
#define CDC_BULK_EP_OUT_MAX_SIZE        64


uint8_t *usb_cdc_itf_desc_get(uint8_t *length);

#endif
