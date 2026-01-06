#include "include.h"
#include "api_codec.h"
#include "usb_audio.h"
#include "bsp_audio_dec.h"

/**********************************************ADPCM**********************************************/
#if VC_RM_DG_ADPCM_DEC_EN
#define ADPCM_SAMPLE_POINTS        (256)    //unit: 16bit
#define ADPCM_DEC_IBUF_LEN         (4+128)  //2byte presample + 1byte index + 1byte reserve + 128byte(4:1)
#define ADPCM_DEC_OBUF_LEN         (ADPCM_SAMPLE_POINTS)
static uint8_t adpcm_dec_ibuf[ADPCM_DEC_IBUF_LEN] AT(.dec.adpcm.ibuf);
static int16_t adpcm_dec_obuf[ADPCM_DEC_OBUF_LEN] AT(.dec.adpcm.obuf);
static bool adpcm_dec_en = 0;
static void bsp_audio_dec_adpcm_init(void)
{
    memset(adpcm_dec_ibuf, 0, sizeof(adpcm_dec_ibuf));
    memset(adpcm_dec_obuf, 0, sizeof(adpcm_dec_obuf));
    adpcm_sample_idx_reset();
}

static void bsp_audio_dec_adpcm_deinit(void)
{
    adpcm_dec_en = 0;
}

void bsp_set_audio_dec_adpcm_en(bool en)
{
    adpcm_dec_en = en;
}

bool bsp_get_audio_dec_adpcm_en(void)
{
    return adpcm_dec_en;
}
#endif //VC_RM_DG_ADPCM_DEC_EN

/**********************************************mSBC**********************************************/
#if VC_RM_DG_mSBC_DEC_EN
#define mSBC_SAMPLE_POINTS        (120)    //unit: 16bit
#define mSBC_DEC_OBUF_LEN         (mSBC_SAMPLE_POINTS)
static int16_t msbc_dec_obuf[mSBC_DEC_OBUF_LEN] AT(.dec.msbc.obuf);
static bool msbc_dec_en = 0;
static void bsp_audio_dec_msbc_init(void)
{
    memset(msbc_dec_obuf, 0, sizeof(msbc_dec_obuf));
    sbc_decode_init(1);
}

static void bsp_audio_dec_msbc_deinit(void)
{
    msbc_dec_en = 0;
}

void bsp_set_audio_dec_msbc_en(bool en)
{
    msbc_dec_en = en;
}

bool bsp_get_audio_dec_msbc_en(void)
{
    return msbc_dec_en;
}
#endif //VC_RM_DG_mSBC_DEC_EN

/**********************************************SBC**********************************************/
#if VC_RM_DG_SBC_DEC_EN
#define SBC_SAMPLE_POINTS        (96)    //unit: 16bit
#define SBC_DEC_OBUF_LEN         (SBC_SAMPLE_POINTS)
static int16_t sbc_dec_obuf[SBC_DEC_OBUF_LEN] AT(.dec.SBC.obuf);
static bool sbc_dec_en = 0; 
static void bsp_audio_dec_sbc_init(void)
{
    memset(sbc_dec_obuf, 0, sizeof(sbc_dec_obuf));
    sbc_decode_init(0);
}
static void bsp_audio_dec_sbc_deinit(void)
{
    sbc_dec_en = 0;
}

void bsp_set_audio_dec_sbc_en(bool en)
{
    sbc_dec_en = en;
}

bool bsp_get_audio_dec_sbc_en(void)
{
    return sbc_dec_en;
}
#endif //VC_RM_DG_SBC_DEC_EN

/*******************************************audio dec init*******************************************/
AT(.text.audio_dec)
void bsp_audio_dec_init(void)
{
#if VC_RM_DG_ADPCM_DEC_EN
    bsp_audio_dec_adpcm_init();
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
    bsp_audio_dec_msbc_init();   
#endif //VC_RM_DG_mSBC_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
    bsp_audio_dec_sbc_init();
#endif //VC_RM_DG_SBC_DEC_EN
}

/*******************************************audio dec close*******************************************/
AT(.text.audio_dec)
void bsp_audio_dec_deinit(void)
{
#if VC_RM_DG_ADPCM_DEC_EN
    bsp_audio_dec_adpcm_deinit();
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
    bsp_audio_dec_msbc_deinit();   
#endif //VC_RM_DG_mSBC_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
    bsp_audio_dec_sbc_deinit();
#endif //VC_RM_DG_SBC_DEC_EN
}

/*******************************************audio dec proc*******************************************/
AT(.com_text.audio_dec)
void bsp_audio_dec_proc(uint8_t *ble_ibuf, uint16_t ble_ilen)
{
#if VC_RM_DG_ADPCM_DEC_EN
    if (adpcm_dec_en) {
        //ble_ibuf: 2byte seq, 1byte id，2byte prev, 1byte index, 128byte adpcm data
        //adpcm_dec_ibuf:  2byte presample + 1byte index + 1byte reserve + 128byte
        adpcm_dec_ibuf[0] = ble_ibuf[4];
        adpcm_dec_ibuf[1] = ble_ibuf[3];
        adpcm_dec_ibuf[2] = ble_ibuf[5];
        adpcm_dec_ibuf[3] = 0;
        memcpy(&adpcm_dec_ibuf[4], &ble_ibuf[6], 128);
        adpcm_decode_block_big((uint8_t *)adpcm_dec_obuf, (uint8_t *)adpcm_dec_ibuf, ADPCM_SAMPLE_POINTS);
        usb_audio_mic_stream(adpcm_dec_obuf, ADPCM_SAMPLE_POINTS);
    }
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
    if (msbc_dec_en) {
        sbc_decode_frame(ble_ibuf, ble_ilen, (uint8_t *)msbc_dec_obuf);
        usb_audio_mic_stream(msbc_dec_obuf, mSBC_SAMPLE_POINTS);
    }
#endif //VC_RM_DG_mSBC_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
    if (sbc_dec_en) {
        sbc_decode_frame(ble_ibuf, ble_ilen, (uint8_t *)sbc_dec_obuf);
        usb_audio_mic_stream(sbc_dec_obuf, SBC_SAMPLE_POINTS);
    }
#endif //VC_RM_DG_mSBC_DEC_EN
}
