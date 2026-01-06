#ifndef _USB_DESC_H
#define _USB_DESC_H

#include "usb_hid_desc.h"
#include "usb_audio_desc.h"
#include "usb_vendor_desc.h"

#define USB_DEV_VID                     0x0642
#define USB_DEV_PID                     0x0001
#define USB_DEV_LANG_ID                 0x0409
#define USB_DESC_MANUF                  "AB2030"
#define USB_DESC_PRODUCT                "AB203X_WIRELESS"
#define USB_DESC_SERIAL                 "0001"

//Descriptor String Index
#define STR_LANGUAGE_ID                 0
#define STR_MANUFACTURER                1
#define STR_PRODUCT                     2
#define STR_SERIAL_NUM                  3

#define USB_MAX_STR_DESC_SIZE           64

enum{
#if USB_HID_EN
    USB_HID_ITF_INDEX,
#endif
#if USB_AUDIO_EN
    USB_AUDIO_CTL_ITF_INDEX,
#if USB_MIC_EN
    USB_AUDIO_STREAM_MIC_ITF_INDEX,
#endif
#endif
#if USB_VENDOR_EN
    USB_VENDOR_ITF_INDEX,
#endif
    USB_ITF_NUM,
};

uint8_t *usb_get_device_descriptor(uint8_t *length);
uint8_t *usb_get_cfg_descriptor(uint8_t *length);
uint8_t *usb_get_lang_id_str_descriptor(uint8_t *length);
uint8_t *usb_get_manufacturer_str_descriptor(uint8_t *length);
uint8_t *usb_get_product_str_descriptor(uint8_t *length);
uint8_t *usb_get_serial_str_descriptor(uint8_t *length);


#endif
