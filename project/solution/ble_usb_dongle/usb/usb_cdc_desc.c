#include "include.h"
#include "usb_desc.h"
#include "usb_cdc_desc.h"

#if USB_CDC_EN

//Config descriptor: CDC
AT(.usbdev.com.table)
const u8 desc_config_cdc[] = {
#if USB_ASSN_EN
    //IAD Descriptor:
    0x08,
    0x0B,
    USB_CDC_INTR_ITF_INDEX,
    0x02,
    0x02,
    0x02,
    0x01,
    0x00,
#endif

    //Interface Descriptor:
    9,                          // Num bytes of this descriptor
    INTERFACE_DESCRIPTOR,       // Interface descriptor type
    USB_CDC_INTR_ITF_INDEX,     // Interface Number
    0,                          // Alternate interface number
    1,                          // Num endpoints of this interface
    0x02,                       // Interface Sub Class: CDC
    0x02,                       // Interface Sub Class:Abstract Control Model
    0x01,                       // Class specific protocol:Common AT Commands
    0,                          // Index of Interface string descriptor

    //CS_INTERFACE
    5,                          // Num bytes of this descriptor
    CS_INTERFACE,               // CS_INTERFACE
    HEADER_FUNC_DESCRIPTOR,     // Header functional descriptor
    0x10,                       // CDC Version
    0x01,

    5,                          // Num bytes of this descriptor
    CS_INTERFACE,               // CS_INTERFACE
    CALL_MANAGE_DESCRIPTOR,     // Call management functional descriptor
    0x00,                       // bmCapabilities
    0x00,                       // bDataInterface

    4,                          // Num bytes of this descriptor
    CS_INTERFACE,               // CS_INTERFACE
    ABSTRACT_CTL_DESCRIPTOR,    // Abstract control management functional descriptor
    0x02,                       // bmCapabilities

    5,                          // Num bytes of this descriptor
    CS_INTERFACE,               // CS_INTERFACE
    UNION_FUNC_DESCRIPTOR,      // Union functional descriptor
    USB_CDC_INTR_ITF_INDEX,     // MasterInterface
    USB_CDC_BULK_ITF_INDEX,     // SlaveInterface


    //EndpointDescriptor:
    7,                                  // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,                // Endpoint descriptor type
    USB_CDC_INTR_EP_INDEX | 0x80,       // Endpoint number
    EP_TYPE_INTR,                       // Interrupt Endpoint
    BYTE0(CDC_INTR_EP_IN_MAX_SIZE),     // Maximum packet size
    BYTE1(CDC_INTR_EP_IN_MAX_SIZE),
    0x10,                               // Poll every 16 msec seconds

    //Interface Descriptor:
    9,                                  // Num bytes of this descriptor
    INTERFACE_DESCRIPTOR,               // Interface descriptor type
    USB_CDC_BULK_ITF_INDEX,             // Interface Number
    0,                                  // Alternate interface number
    2,                                  // Num endpoints of this interface
    0x0A,                               // Interface Sub Class: CDC
    0x00,                               // Interface Sub Class
    0x00,                               // Class specific protocol
    0,                                  // Index of Interface string descriptor

    //EndpointDescriptor:
    7,                                  // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,                // Endpoint descriptor type
    USB_CDC_BULK_EP_IN_INDEX | 0x80,    // Endpoint number
    EP_TYPE_BULK,                       // Interrupt Endpoint
    BYTE0(CDC_BULK_EP_IN_MAX_SIZE),     // Maximum packet size
    BYTE1(CDC_BULK_EP_IN_MAX_SIZE),
    0x0,                                // bInterval

     //EndpointDescriptor:
    7,                                  // Num bytes of this descriptor
    ENDPOINT_DESCRIPTOR,                // Endpoint descriptor type
    USB_CDC_BULK_EP_OUT_INDEX,          // Endpoint number
    EP_TYPE_BULK,                       // Interrupt Endpoint
    BYTE0(CDC_BULK_EP_OUT_MAX_SIZE),    // Maximum packet size
    BYTE1(CDC_BULK_EP_OUT_MAX_SIZE),
    0x0,                                // bInterval
};



uint8_t *usb_cdc_itf_desc_get(uint8_t *length)
{
    *length = sizeof(desc_config_cdc);
    return (u8 *)desc_config_cdc;
}

#endif
