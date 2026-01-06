#ifndef _USB_DEV_ENUM_H
#define _USB_DEV_ENUM_H

#include "include.h"

bool usb_ep0_ctl_callback(spb_wrap_t *spb, uint8_t *data, uint16_t data_len);
void ude_state_reset(ude_cb_t *ude);

#endif
