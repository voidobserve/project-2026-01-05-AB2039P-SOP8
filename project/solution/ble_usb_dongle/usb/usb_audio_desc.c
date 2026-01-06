#include "include.h"
#include "usb_com.h"
#include "usb_audio.h"
#include "usb_desc.h"
#include "usb_audio_desc.h"

#if USB_AUDIO_EN
AT(.rodata.usb.desc)
const uint8_t desc_itf_header_audio[] = {
    0x09,                                   /* Num bytes of this descriptor */
    INTERFACE_DESCRIPTOR,                   /* Interface descriptor type */
    USB_AUDIO_CTL_ITF_INDEX,                /* Interface Number*/
    0x00,                                   /* Alternate interface number */
    0x00,                                   /* Num endpoints of this interface */
    0x01,                                   /* Interface Class: Audio */
    0x01,                                   /* Interface Sub Class: Audio Control */
    0x00,                                   /* Class specific protocol */
    0x00,                                   /* Index of Interface string descriptor */

#if (AUDIO_TOTAL_NR == 1)
    0x09,                                   /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,             /* Audio Control Interface descriptor type */
    UA_HEADER_DESCRIPTOR,                   /* HEADER descriptor */
    0x00,                                   /* Audio 1.0 LSB */
    0x01,                                   /* Audio 1.0 MSB */
    0x27,                                   /* Total Size of Audio Control LSB, sizeof(desc_itf_header_audio) - 9*/
    0x00,                                   /* Total Size of Audio Control MSB */
    AUDIO_TOTAL_NR,                         /* Num Audio OUT Streaming Interface */
    USB_AUDIO_CTL_ITF_INDEX + 1,            /* Number of Audio Streaming Nr1*/
#elif (AUDIO_TOTAL_NR == 2)
    0x0a,                                   /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,             /* Audio Control Interface descriptor type */
    UA_HEADER_DESCRIPTOR,                   /* HEADER descriptor */
    0x00,                                   /* Audio 1.0 LSB */
    0x01,                                   /* Audio 1.0 MSB */
    0x46,                                   /* Total Size of Audio Control LSB, sizeof(desc_itf_header_audio) - 9 */
    0x00,                                   /* Total Size of Audio Control MSB */
    AUDIO_TOTAL_NR,                         /* Num Audio OUT Streaming Interface */
    USB_AUDIO_STREAM_MIC_ITF_INDEX,         /* Number of Audio Streaming Nr1*/
    USB_AUDIO_STREAM_SPEAKER_ITF_INDEX,     /* Number of Audio Streaming Nr2*/
#endif // AUDIO_TOTAL_NR

#if USB_MIC_EN
    0x0c,                                   /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,             /* Audio Control Interface descriptor type */
    UA_INPUT_TERM_DESCRIPTOR,               /* INPUT_TERMINAL descriptor */
    UA_MIC_INPUT_ID,                        /* Terminal ID */
    0x01,                                   /* Terminal Type: 0x0201 = Microphone  LSB */
    0x02,                                   /* Terminal Type: 0x0201 = Microphone  MSB */
    0x00,                                   /* Associate Terminal */
    0x01,                                   /* NrChannel, Single */
    0x00,                                   /* Channel Config LSB */
    0x00,                                   /* Channel Config MSB */
    0x00,                                   /* Channel Name */
    0x00,                                   /* Index of Channel String */

    0x09,                                   /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,             /* Audio Control Interface descriptor type */
    UA_FEATURE_UNIT_DESCRIPTOR,             /* FEATURE_UNIT descriptor */
    UA_MIC_UNIT_ID,                         /* bUnitID, Feature Unit ID */
    UA_MIC_INPUT_ID,                        /* bSourceID */
    0x01,                                   /* Control Size */
    0x03,                                   /* bmaControls(0): D0(Mute) D1(Volume),  mute and volume control supported for master channel */
    0x00,                                   /* bmaControls(1) */
    0x00,                                   /* Index of Feature String */

    0x09,                                   /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,             /* Audio Control Interface descriptor type */
    UA_OUTPUT_TERM_DESCRIPTOR,              /* OUTPUT_TERMINAL descriptor */
    UA_MIC_OUTPUT_ID,                       /* Terminal ID, The same of mic stream TID */
    0x01,                                   /* Terminal Type: 0x0101 - USB Streaming LSB */
    0x01,                                   /* Terminal Type: 0x0101 - USB Streaming MSB */
    0x00,                                   /* Associate Terminal */
    UA_MIC_UNIT_ID,                         /* bSourceID */
    0x00,                                   /* Index of Channel String */
#endif // USB_MIC_EN
};

#if USB_MIC_EN
AT(.rodata.usb.desc)
const uint8_t desc_itf_mic_stream_audio[] = {
    0x09,                               /* Num bytes of this descriptor */
    INTERFACE_DESCRIPTOR,               /* Interface descriptor type */
    USB_AUDIO_STREAM_MIC_ITF_INDEX,     /* Interface Number*/
    0x00,                               /* Alternate interface number */
    0x00,                               /* Num endpoints of this interface */
    0x01,                               /* Interface Class: Audio */
    0x02,                               /* Interface Sub Class: Audio Streaming */
    0x00,                               /* Class specific protocol: */
    0x00,                               /* Index of Interface string descriptor */

    0x09,                               /* Num bytes of this descriptor */
    INTERFACE_DESCRIPTOR,               /* Interface descriptor type */
    USB_AUDIO_STREAM_MIC_ITF_INDEX,     /* Interface Number*/
    0x01,                               /* Alternate interface number */
    0x01,                               /* Num endpoints of this interface */
    0x01,                               /* Interface Class: Audio */
    0x02,                               /* Interface Sub Class: Audio Streaming */
    0x00,                               /* Class specific protocol: */
    0x00,                               /* Index of Interface string descriptor */

    0x07,                               /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,         /* Audio Control Interface descriptor type */
    UA_STREAM_GENERAL_DESCRIPTOR,       /* AS_GENERAL descriptor */
    UA_MIC_OUTPUT_ID,                   /* Terminal ID of Input Terminal */
    0x01,                               /* Interface Delay */
    0x01,                               /* PCM Format 0x0001 = 16bit LSB */
    0x00,                               /* PCM Format MSB */

    0x0b,                               /* Num bytes of this descriptor */
    AUDIO_INTERFACE_DESCRIPTOR,         /* Audio Control Interface descriptor type */
    UA_STREAM_FORMAT_DESCRIPTOR,        /* FORMAT_TYPE descriptor */
    0x01,                               /* FORMAT_TYPE */
    0x01,                               /* NrChannel = 1，MIC used single channel */
    0x02,                               /* frame = 2byte */
    0x10,                               /* 16bit per sample */
    0x01,                               /* One Frequency supported */
    BYTE0(AUDIO_SPEAKER_MIC),           /* Sample Rate */
    BYTE1(AUDIO_SPEAKER_MIC),
    BYTE2(AUDIO_SPEAKER_MIC),

    //EndpointDescriptor:
    0x09,                               /* Num bytes of this descriptor */
    ENDPOINT_DESCRIPTOR,                /* Endpoint descriptor type */
    USB_AUDIO_EP_IN_INDEX | 0x80,       /* Endpoint number */
    AUDIO_ISOIN_TYPE,                   /* ISO endpoint, ASY */
    BYTE0(AUDIO_ISOIN_SIZE),            /* Maximum packet size，sample point bytes by 1ms */
    BYTE1(AUDIO_ISOIN_SIZE),
    0x01,                               /* ISO 1ms */
    0x00,                               /* Refresh */
    0x00,                               /* SynchAddress */

    0x07,                               /* Num bytes of this descriptor */
    AUDIO_ENDPOINT_DESCRIPTOR,          /* CS_ENDPOINT descriptor */
    0x01,                               /* EP_GENENAL descriptor */
    0x01,                               /* ATTR */
    0x00,                               /* LockDelayUnits */
    0x00,                               /* LockDelay LSB */
    0x00,                               /* LockDelay MSB */
};
#endif // USB_MIC_EN


uint8_t *usb_audio_itf_header_desc_get(uint8_t *length)
{
    *length = sizeof(desc_itf_header_audio);
    return (u8 *)desc_itf_header_audio;
}

#if USB_MIC_EN
uint8_t *usb_audio_itf_mic_stream_desc_get(uint8_t *length)
{
    *length = sizeof(desc_itf_mic_stream_audio);
    return (u8 *)desc_itf_mic_stream_audio;
}
#endif // USB_MIC_EN

#endif





