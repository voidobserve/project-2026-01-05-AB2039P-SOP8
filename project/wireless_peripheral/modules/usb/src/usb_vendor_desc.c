#include "include.h"
#include "usb_desc.h"

#if USB_VENDOR_EN

AT(.rodata.usb.desc)
const uint8_t desc_itf_vendor[] = {
    //Interface Descriptor:
    9,                              /* Num bytes of this descriptor */
    INTERFACE_DESCRIPTOR,           /* Interface descriptor type */
    USB_VENDOR_ITF_INDEX,           /* Interface Number*/
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
    BYTE0(VENDOR_REPORT_DESC_SIZE), /* Len of Report descriptor ,sizeof(desc_vendor_report) */
    BYTE1(VENDOR_REPORT_DESC_SIZE),

    //Endpoint Descriptor: (OUT)
    0x07,                           /* Num bytes of this descriptor */
    ENDPOINT_DESCRIPTOR,            /* Endpoint descriptor type */
    USB_VENDOR_EP_OUT_INDEX,        /* Endpoint address: */
    EP_TYPE_INTR,                   /* Endpoint type: */
    BYTE0(VENDOR_EP_OUT_MAX_SIZE),  /* Maximum packet size */
    BYTE1(VENDOR_EP_OUT_MAX_SIZE),
    0x01,                           /* Poll */

    //Endpoint Descriptor: (IN)
    0x07,                           /* Num bytes of this descriptor */
    ENDPOINT_DESCRIPTOR,            /* Endpoint descriptor type */
    USB_VENDOR_EP_IN_INDEX | 0x80,  /* Endpoint address: */
    EP_TYPE_INTR,                   /* Endpoint type: */
    BYTE0(VENDOR_EP_IN_MAX_SIZE),   /* Maximum packet size */
    BYTE1(VENDOR_EP_IN_MAX_SIZE),
    0x01,                           /* Poll */
};

//HID report
AT(.rodata.usb.desc)
const u8 desc_vendor_report[] = {
    HID_REP_MAP_CONSUMER(HID_REP_ID_CONSUMER),
    HID_REP_MAP_APP(HID_REP_ID_APP),
    HID_REP_MAP_DBG(HID_REP_ID_DGB),
};


/* Return the desc to the lib by rewriting this func */
uint8_t *usb_vendor_itf_desc_get(uint8_t *length)
{
    if (VENDOR_REPORT_DESC_SIZE != sizeof(desc_vendor_report)){
        printf("--->desc_hid_report_size_err: %d,%d\n", VENDOR_REPORT_DESC_SIZE, sizeof(desc_vendor_report));
        while(1);
    }

    *length = sizeof(desc_itf_vendor);
    return (u8 *)desc_itf_vendor;
}

uint8_t *usb_vendor_report_desc_get(uint8_t *length)
{
    *length = sizeof(desc_vendor_report);
    return (u8 *)desc_vendor_report;
}

#endif
