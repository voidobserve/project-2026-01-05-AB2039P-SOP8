#ifndef _USB_COM_H
#define _USB_COM_H

typedef void (*ep_handler)(void);

typedef struct {
    ep_handler eptx_handler[EP_CNT];           //EP TX
    ep_handler eprx_handler[EP_CNT];           //EP RX
} ep_cb_t;

extern ep_cb_t ep_cb;

typedef struct _ude_t {
    u8 devaddr;                     //当前设备地址
    u8 cfgval;                      //当前配置

    volatile u8 error       : 1;    //出错
} ude_cb_t;

extern ude_cb_t ude_cb;

void usb_ep_callback_register(ep_handler callback, uint8_t ep_index, uint8_t ep_dir);
bool usb_ep_start_transfer(epcb_t *epcb, uint len);
void usb_device_init(void);
void usb_device_deinit(void);
void usb_ep_reset_deal(void);
void usb_device_process(void);

#endif // _USB_COM_H
