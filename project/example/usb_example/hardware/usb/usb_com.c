#include "include.h"
#include "usb_com.h"
#include "usb_dev_enum.h"
#include "usb_hid.h"
#include "usb_cdc.h"
#include "usb_cdc_desc.h"
#include "usb_usr_cdc.h"

ep_cb_t ep_cb;

usb_handler_cb_t usb_handler_cb;

void usb_ep_callback_register(ep_handler callback, uint8_t ep_index, uint8_t ep_dir)
{
    if (ep_index < EP_CNT) {
        if (ep_dir) {
            ep_cb.eptx_handler[ep_index] = callback;
        } else {
            ep_cb.eprx_handler[ep_index] = callback;
        }
    }
}

bool usb_ep_start_transfer(epcb_t *epcb, uint len)
{
    if (epcb && len) {
        epcb->xptr = epcb->buf;
        epcb->xlen = len;
        epcb->xcnt = 0;
        epcb->first_pkt = 1;
        return usb_ep_transfer(epcb);
    }

    return false;
}

void usb_ep_reset_deal(void)
{
#if USB_HID_EN
    usb_int_ep_reset();
#endif

#if USB_AUDIO_EN
    usb_isoc_ep_reset();
#endif

#if USB_CDC_EN
    usb_cdc_ep_reset();
#endif

}

AT(.usbdev.com)
void usb_device_process(void)
{
#if USB_HID_EN
    usb_hid_process();
#endif
#if USB_AUDIO_EN
    usb_audio_process();
#endif

#if USB_CDC_EN
    usb_cdc_process();
#endif
}

bool usb_event_callback(uint8_t event, spb_wrap_t *spb, uint8_t *data, uint8_t data_len)
{
    switch (event){
        case MSG_USB_EP_RESET:
            ude_state_reset(&ude_cb);
            usb_ep_reset_deal();
            break;

        case MSG_USB_EP0_CTL_FLOW:
            return usb_ep0_ctl_callback(spb, data, data_len);

        case MSG_USB_EP1_IN:
            if (ep_cb.eptx_handler[1]) {
                ep_cb.eptx_handler[1]();
            }
            break;

        case MSG_USB_EP1_OUT:
            if (ep_cb.eprx_handler[1]) {
                ep_cb.eprx_handler[1]();
            }
            break;

        case MSG_USB_EP2_IN:
            if (ep_cb.eptx_handler[2]) {
                ep_cb.eptx_handler[2]();
            }
            break;

        case MSG_USB_EP2_OUT:
            if (ep_cb.eprx_handler[2]) {
                ep_cb.eprx_handler[2]();
            }
            break;

        case MSG_USB_EP3_IN:
            if (ep_cb.eptx_handler[3]) {
                ep_cb.eptx_handler[3]();
            }
            break;

        case MSG_USB_EP3_OUT:
            if (ep_cb.eprx_handler[3]) {
                ep_cb.eprx_handler[3]();
            }
            break;

        default:
            return false;
    }

    return true;
}

bool usb_device_init_handler(void)
{
#if USB_HID_EN
    udh_init();
#endif
#if USB_AUDIO_EN
    uda_init();
#endif

#if USB_CDC_EN
    usb_cdc_init();
#endif

    return true;
}

void usb_device_init(void)
{
    usb_handler_cb.init = usb_device_init_handler;
    usb_handler_cb.evt = usb_event_callback;

    usb_handler_register(&usb_handler_cb);

#if USB_AUDIO_EN
    usb_audio_init();
#endif
}

void usb_device_deinit(void)
{
#if USB_AUDIO_EN
    usb_audio_deinit();
#endif

#if USB_HID_EN
    usb_hid_deinit();
#endif // USB_HID_EN

#if USB_CDC_EN
    usb_cdc_deinit();
#endif // USB_CDC_EN
}
