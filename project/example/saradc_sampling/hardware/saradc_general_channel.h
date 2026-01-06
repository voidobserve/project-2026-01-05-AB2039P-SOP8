#ifndef _SARADC_GENERAL_CHANNEL_H
#define _SARADC_GENERAL_CHANNEL_H
#include "include.h"

#define REFERENCE_VOLTAGE_CHANNEL       ADC_CHANNEL_BG
#define REFERENCE_VOLTAGE_VALUE         pmu_get_vbg_mv()

void saradc_general_ch_add_ch_test_0(void);
void saradc_general_ch_get_value_test_0(void);
void saradc_general_ch_add_ch_test_1(void);
void saradc_general_ch_get_value_test_1(void);
void saradc_general_ch_user_proc(void);

void saradc_general_ch_proc(void);
#endif // _SARADC_GENERAL_CHANNEL_H
