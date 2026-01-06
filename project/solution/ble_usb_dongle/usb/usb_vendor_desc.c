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
	0x05, 0x0C, 		//  Usage Page (Consumer)
	0x09, 0x01, 		//  Usage (Consumer Control)
	0xA1, 0x01, 		//  Collection (Application)
	0x1A, 0x00, 0x00,	//	Usage Minimum (Unassigned)
	0x2A, 0x9C, 0x02,	//	Usage Maximum (AC Distribute Vertically)
	0x16, 0x00, 0x00,	//	Logical minimum (0)
	0x26, 0x9C, 0x02,	//	Logical maximum (668)
	0x75, 0x10, 		//	Report Size (16)
	0x95, 0x01, 		//	Report Count (1)
	0x81, 0x00, 		//	Input (Data,Array,Absolute,Bit Field)
	0xC0,				//  End Collection
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
