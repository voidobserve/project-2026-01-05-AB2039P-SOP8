#ifndef __USB_USR_AUDIO_H
#define __USB_USR_AUDIO_H


typedef struct {
    void        (*init)                 (void);
    void        (*mic_start)            (void);
    void        (*mic_stop)             (void);
    void        (*mic_mute_set)         (bool mute);
    void        (*mic_vol_set)          (uint16_t vol);
    uint32_t    (*mic_data_request)     (uint8_t *pcm16_data, uint32_t size);
} usb_usr_audio_cb_t;

extern usb_usr_audio_cb_t usb_usr_audio_cb;


bool usb_usr_audio_mic_is_start(void);
void usb_usr_audio_mic_decode(void);
bool usb_usr_audio_mic_write(void *mic_data, uint32_t size);

#endif // __USB_USR_AUDIO_H
