#include "include.h"
#include "usb_desc.h"
#include "usb_hid_desc.h"

#if USB_HID_EN

//Config descriptor: HID
AT(.usbdev.com.table)
const u8 desc_config_hid[] = {
 #if USB_ASSN_EN
    //IAD Descriptor:
    0x08,
    0x0B,
    USB_HID_ITF_INDEX,
    0x01,
    0x03,
    0x00,
    0x00,
    0x00,
#endif
    //Interface Descriptor:
    9,                              /* Num bytes of this descriptor */
    INTERFACE_DESCRIPTOR,           /* Interface descriptor type */
    USB_HID_ITF_INDEX,              /* Interface Number*/
    0,                              /* Alternate interface number */
    2,                              /* Num endpoints of this interface */
    0x03,                           /* Interface Sub Class: Human Interface Device */
    0,                              /* Interface Sub Class: */
    0,                              /* Class specific protocol: */
    0,                              /* Index of Interface string descriptor */

    //HID Descriptor:
    9,                              /* Num bytes of this descriptor */
    HID_DESCRIPTOR,                 /* HID Type */
    0x11, 0x01,                     /* HID Class Specification compliance */
    0x00,                           /* Country Code: none */
    0x01,                           /* Number of descriptors to follow */
    HID_REPORT_DESCRIPTOR,          /* Report descriptor type */
    BYTE0(HID_REPORT_DESC_SIZE),    /* Len of Report descriptor ,sizeof(desc_hid_report) */
    BYTE1(HID_REPORT_DESC_SIZE),

    //EndpointDescriptor:
    7,                              /* Num bytes of this descriptor */
    ENDPOINT_DESCRIPTOR,            /* Endpoint descriptor type */
    USB_HID_EP_OUT_INDEX,           /* Endpoint address: */
    EP_TYPE_INTR,                   /* Interrupt Endpoint */
    BYTE0(HID_EP_OUT_MAX_SIZE),     /* Maximum packet size */
    BYTE1(HID_EP_OUT_MAX_SIZE),
    0x01,                           /* Poll every 1 msec seconds */

    //EndpointDescriptor:
    7,                              /* Num bytes of this descriptor */
    ENDPOINT_DESCRIPTOR,            /* Endpoint descriptor type */
    USB_HID_EP_IN_INDEX | 0x80,     /* Endpoint address: */
    EP_TYPE_INTR,                   /* Interrupt Endpoint */
    BYTE0(HID_EP_IN_MAX_SIZE),      /* Maximum packet size */
    BYTE1(HID_EP_IN_MAX_SIZE),
    0x01,                           /* Poll every 1 msec seconds */
};

//HID report
AT(.rodata.usb.desc)
const u8 desc_hid_report[] = {
    0x05, 0x01,         /*  USAGE_PAGE (Generic Desktop) */
    0x09, 0x02,         /*  USAGE (Mouse) */
    0xa1, 0x01,         /*  COLLECTION (Application) */
    0x09, 0x01,         /*  USAGE (Pointer) */
    0xa1, 0x00,         /*  COLLECTION (Physical) */
    0x05, 0x09,         /*  USAGE_PAGE (Button) */
    0x19, 0x01,         /*  USAGE_MINIMUM (Button 1) */
    0x29, 0x03,         /*  USAGE_MAXIMUM (Button 3) */
    0x15, 0x00,         /*  LOGICAL_MINIMUM (0) */
    0x25, 0x01,         /*  LOGICAL_MAXIMUM (1) */
    0x75, 0x01,         /*  REPORT_SIZE (1) */
    0x95, 0x03,         /*  REPORT_COUNT (3) */
    0x81, 0x02,         /*  INPUT (Data,Var,Abs) */
    0x75, 0x05,         /*  REPORT_SIZE (5) */
    0x95, 0x01,         /*  REPORT_COUNT (1) */
    0x81, 0x01,         /*  INPUT (Cnst,Ary,Abs) */
    0x05, 0x01,         /*  USAGE_PAGE (Generic Desktop) */
    0x09, 0x38,         /*  USAGE (Wheel) */
    0x09, 0x30,         /*  USAGE (X) */
    0x09, 0x31,         /*  USAGE (Y) */
    0x15, 0x81,         /*  LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,         /*  LOGICAL_MAXIMUM (127) */
    0x75, 0x08,         /*  REPORT_SIZE (8) */
    0x95, 0x03,         /*  REPORT_COUNT (3) */
    0x81, 0x06,         /*  INPUT (Data,Var,Rel) */
    0xc0,               /*  END_COLLECTION */
    0xc0,               /*  END_COLLECTION */
};

uint8_t *usb_hid_itf_desc_get(uint8_t *length)
{
    if (HID_REPORT_DESC_SIZE != sizeof(desc_hid_report)){
        printf("--->desc_hid_report_size_err: %d,%d\n", HID_REPORT_DESC_SIZE, sizeof(desc_hid_report));
        while(1);
    }

    *length = sizeof(desc_config_hid);
    return (u8 *)desc_config_hid;
}

uint8_t *usb_hid_report_desc_get(uint8_t *length)
{
    *length = sizeof(desc_hid_report);
    return (u8 *)desc_hid_report;
}

#endif
