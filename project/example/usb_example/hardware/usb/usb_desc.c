#include "include.h"
#include "usb_com.h"
#include "usb_desc.h"


AT(.rodata.usb.desc)
const uint8_t usb_device_desc[] = {
    0x12,                   /* Num bytes of the descriptor */
    DEVICE_DESCRIPTOR,      /* Device Descriptor type */
    0x10,                   /* Revision of USB Spec. LSB (in BCD) */
    0x01,                   /* Revision of USB Spec. MSB (in BCD) */
#if USB_ASSN_EN
    0xEF,                   /* Device class code */
    0x02,                   /* Subclass code */
    0x01,                   /* Device protocol */
#elif USB_CDC_EN
    0x02,                   /* Device class code */
    0x00,                   /* Subclass code */
    0x00,                   /* Device protocol */
#else
    0x00,                   /* Device class code */
    0x00,                   /* Subclass code */
    0x00,                   /* Device protocol */
#endif
    0x40,                   /* Max packet size of Endpoint 0: only valid 8, 16, 32 or 64 */

    BYTE0(USB_DEV_VID),     /* idVendor */
    BYTE1(USB_DEV_VID),     /* idVendor */
    BYTE0(USB_DEV_PID),     /* idProduct */
    BYTE1(USB_DEV_PID),     /* idProduct */

    0x00,                   /* Device Revision LSB (in BCD) */
    0x01,                   /* Device Revision MSB (in BCD) */

    STR_MANUFACTURER,       /* Index of Manufacture string descriptor */
    STR_PRODUCT,            /* Index of Product string descriptor */
    STR_SERIAL_NUM,         /* Index of Serial No. string descriptor */
    0x01,
};

AT(.rodata.usb.desc)
const uint8_t usb_cfg_desc[] = {
    0x09,                       /* Num bytes of this descriptor */
    CONFIGURATION_DESCRIPTOR,   /* Configuration descriptor type */
    0x00,                       /* Total size of configuration LSB*/
    0x00,                       /* Total size of configuration MSB*/
    USB_ITF_NUM,                /* Total Interface number*/
    0x01,                       /* Configuration number */
    0x00,                       /* Index of Configuration string descriptor */
    0x80,                       /* Configuration characteristics: BusPowerd */
                                /*      D7:      reserved and should be one in historical version
                                        D6:      self-powered device
                                        D5:      remote wakeup
                                        D4...D0: reserved and should be set to zero */
    0x32,                       /* Max current, unit is 2mA */
};

AT(.rodata.usb.desc)
const uint8_t usb_lang_desc[] = {
    4,
    3,
    BYTE0(USB_DEV_LANG_ID),
    BYTE1(USB_DEV_LANG_ID),
};

AT(.usb_buf.desc)
uint8_t usb_str_desc_buf[USB_MAX_STR_DESC_SIZE];


static void usb_desc_swapper(u8 *dest, char *src, u8 *length)
{
    u8 str_length = 0;
    u8 idx = 0;

    if (src != NULL) {
        while (src[str_length] != '\0' && str_length < USB_MAX_STR_DESC_SIZE) {
            str_length++;
        }

        *length = str_length * 2 + 2;
        dest[idx++] = *length;
        dest[idx++] = 0x03;

        while (*src != '\0') {
            dest[idx++] = *src++;
            dest[idx++] = 0x00;
        }
    }
}

uint8_t *usb_get_device_descriptor(uint8_t *length)
{
    *length = sizeof(usb_device_desc);
	return (u8 *)usb_device_desc;
}

uint8_t *usb_get_cfg_descriptor(uint8_t *length)
{
    *length = sizeof(usb_cfg_desc);
    return (u8 *)usb_cfg_desc;
}

uint8_t *usb_get_lang_id_str_descriptor(uint8_t *length)
{
    *length = sizeof(usb_lang_desc);
    return (u8 *)usb_lang_desc;
}

uint8_t *usb_get_manufacturer_str_descriptor(uint8_t *length)
{
    usb_desc_swapper(usb_str_desc_buf, USB_DESC_MANUF, length);
    return usb_str_desc_buf;
}

uint8_t *usb_get_product_str_descriptor(uint8_t *length)
{
    usb_desc_swapper(usb_str_desc_buf, USB_DESC_PRODUCT, length);
    return usb_str_desc_buf;
}

uint8_t *usb_get_serial_str_descriptor(uint8_t *length)
{
    usb_desc_swapper(usb_str_desc_buf, USB_DESC_SERIAL, length);
    return usb_str_desc_buf;
}
