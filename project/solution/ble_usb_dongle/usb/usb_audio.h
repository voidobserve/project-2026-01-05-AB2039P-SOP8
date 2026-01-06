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
void usb_audio_mic_stream(void *ibuf, u32 sample);

bool uda_get_spk_mute(void);
bool uda_get_spk_volume(u8 req);
void uda_set_isocout_flag(u8 val);
uint8_t uda_get_isocout_flag(void);
void uda_set_spk_mute(u8 val);
void uda_set_spk_volume(u16 val);
void usb_audio_speaker_stream(void *obuf, u32 *sample);

#endif // _USB_AUDIO_H
