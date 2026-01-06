#ifndef __USB_AUDIO_DESC_H
#define __USB_AUDIO_DESC_H


#define USB_AUDIO_EP_IN_INDEX             2

#if VC_RM_DG_ADPCM_DEC_EN
#define AUDIO_SPEAKER_MIC               8000

#elif VC_RM_DG_mSBC_DEC_EN
#define AUDIO_SPEAKER_MIC               16000

#elif VC_RM_DG_SBC_DEC_EN
#define AUDIO_SPEAKER_MIC               16000

#else
#define AUDIO_SPEAKER_MIC               16000
#endif

#define AUDIO_ISOIN_SIZE                (AUDIO_SPEAKER_MIC / 1000 * 1 * 2)      /* Only support Mono and PCM16bits */
#define AUDIO_TOTAL_NR                  (USB_MIC_EN)

#define AUDIO_ISOIN_TYPE                0x05                                    /* bit[1:0] 0:ctrl 1:isoc 2:bulk 3:int */

enum{
    UA_MIC_INPUT_ID = 0x01,
    UA_MIC_UNIT_ID,
    UA_MIC_OUTPUT_ID,
};


uint8_t *usb_audio_itf_mic_stream_desc_get(uint8_t *length);
uint8_t *usb_audio_itf_header_desc_get(uint8_t *length);

#endif
