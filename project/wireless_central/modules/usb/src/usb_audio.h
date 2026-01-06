#ifndef _USB_AUDIO_H
#define _USB_AUDIO_H

typedef struct _uda_t {
#if USB_MIC_EN
    epcb_t *isoc_in;                         //mic endpoint
    u8 flag_isocin_int;
    u8 flag_isocin_en;
    u16 mic_vol;
    u8 mic_mute;
#endif
} uda_t;


void uda_init(void);
void usb_isoc_ep_reset(void);
void usb_audio_init(void);
void usb_audio_deinit(void);
void usb_audio_process(void);

void uda_set_isocin_flag(u8 val);
uint8_t uda_get_isocin_flag(void);
bool uda_get_mic_mute(void);
bool uda_get_mic_volume(u8 req);
void uda_set_mic_mute(u8 val);
void uda_set_mic_volume(u16 val);

#endif // _USB_AUDIO_H
