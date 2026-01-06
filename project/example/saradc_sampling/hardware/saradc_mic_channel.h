#ifndef _SARADC_MIC_CHANNEL_H
#define _SARADC_MIC_CHANNEL_H
#include "include.h"

int32_t saradc_mic_power_get(void);
void saradc_mic_ch_isr(void);
void saradc_mic_ch_init(void);
void saradc_mic_ch_start(void);
void saradc_mic_ch_stop(void);
void saradc_mic_ch_proc(void);
#endif // _SARADC_MIC_CHANNEL_H
