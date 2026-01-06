#ifndef _BSP_AUDIO_DEC_H
#define _BSP_AUDIO_DEC_H

#if VC_RM_DG_ADPCM_DEC_EN
void bsp_set_audio_dec_adpcm_en(bool en);
bool bsp_get_audio_dec_adpcm_en(void);
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_mSBC_DEC_EN
void bsp_set_audio_dec_msbc_en(bool en);
bool bsp_get_audio_dec_msbc_en(void);
#endif //VC_RM_DG_ADPCM_DEC_EN

#if VC_RM_DG_SBC_DEC_EN
void bsp_set_audio_dec_sbc_en(bool en);
bool bsp_get_audio_dec_sbc_en(void);
#endif //VC_RM_DG_ADPCM_DEC_EN

void bsp_audio_dec_init(void);
void bsp_audio_dec_deinit(void);
void bsp_audio_dec_proc(uint8_t *ble_ibuf, uint16_t ble_ilen);
#endif // _BSP_AUDIO_DEC_H