#ifndef __USB_AUDIO_DESC_H
#define __USB_AUDIO_DESC_H


#define USB_AUDIO_EP_IN_INDEX           2       /* Endpoint index:  2 */
#define AUDIO_SPEAKER_MIC               8000    /* Sample rate:     16k */

#define AUDIO_ISOIN_SIZE                (AUDIO_SPEAKER_MIC / 1000 * 1 * 2)
#define AUDIO_TOTAL_NR                  (USB_MIC_EN)
#define AUDIO_ISOIN_TYPE                0x05

enum{
    UA_MIC_INPUT_ID = 0x01,
    UA_MIC_UNIT_ID,
    UA_MIC_OUTPUT_ID,
};


uint8_t *usb_audio_itf_mic_stream_desc_get(uint8_t *length);
uint8_t *usb_audio_itf_header_desc_get(uint8_t *length);

#endif
