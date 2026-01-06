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
    9,                          // Num bytes of this descriptor
    INTERFACE_DESCRIPTOR,       // Interface descriptor type
    USB_HID_ITF_INDEX,          // Interface Number
    0,                          // Alternate interface number
    2,                          // Num endpoints of this interface
    0x03,                       // Interface Sub Class: Human Interface Device
    0,                          // Interface Sub Class:
    0,                          // Class specific protocol:
    0,                          // Index of Interface string descriptor

    //HID Descriptor:
    9,                          // Num bytes of this descriptor
    HID_DESCRIPTOR,             // HID Type
    0x01, 0x02,                 // HID Class Specification compliance
    0x00,                       // Country Code: none
    0x01,                       // Number of descriptors to follow
    HID_REPORT_DESCRIPTOR,      // Report descriptor type
    BYTE0(HID_REPORT_DESC_SIZE),// Len of Report descriptor ,sizeof(desc_hid_report)
    BYTE1(HID_REPORT_DESC_SIZE),

    //EndpointDescriptor:
    7,                          // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,        // Endpoint descriptor type
    USB_HID_EP_OUT_INDEX,       // Endpoint number: HID OUT
    EP_TYPE_INTR,               // Interrupt Endpoint
    BYTE0(HID_EP_OUT_MAX_SIZE), // Maximum packet size
    BYTE1(HID_EP_OUT_MAX_SIZE),
    0x01,                       // Poll every 1 msec seconds

    //EndpointDescriptor:
    7,                          // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,        // Endpoint descriptor type
    USB_HID_EP_IN_INDEX | 0x80, // Endpoint number: HID IN
    EP_TYPE_INTR,               // Interrupt Endpoint
    BYTE0(HID_EP_IN_MAX_SIZE),  // Maximum packet size
    BYTE1(HID_EP_IN_MAX_SIZE),
    0x01,                       // Poll every 1 msec seconds
};

//HID report
AT(.usbdev.com.table)
const u8 desc_hid_report[] = {
    0x05, 0x01,
    0x09, 0x06,
    0xa1, 0x01,
    0x05, 0x07,
    0x19, 0xe0,
    0x29, 0xe7,
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x08,
    0x81, 0x02,
    0x95, 0x01,
    0x75, 0x08,
    0x81, 0x03,
    0x95, 0x05,
    0x75, 0x01,
    0x05, 0x08,
    0x19, 0x01,
    0x29, 0x05,
    0x91, 0x02,
    0x95, 0x01,
    0x75, 0x03,
    0x91, 0x03,
    0x95, 0x06,
    0x75, 0x08,
    0x15, 0x00,
    0x25, 0xFF,
    0x05, 0x07,
    0x19, 0x00,
    0x29, 0x65,
    0x81, 0x00,
    0xc0
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
