#include "include.h"
#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "saradc_sampling.h"
#include "saradc_general_channel.h"

#if SARADC_GENERAL_EN

#define ADC_CHANNEL_TEST0       ADC_CHANNEL_PB6
#define ADC_CHANNEL_TEST1       ADC_CHANNEL_VBAT

void saradc_general_ch_add_ch_test_0(void)
{
    saradc_add_channel(ADC_CHANNEL_TEST0, SARADC_PULLUP_DIS);
    saradc_kick_start();
}

void saradc_general_ch_get_value_test_0(void)
{
    uint16_t ch6_adc_val;
    ch6_adc_val = saradc_get_channel_value(ADC_CHANNEL_TEST0);
    printf("test_0 ch6_adc_val:%d\n", ch6_adc_val);
}

void saradc_general_ch_add_ch_test_1(void)
{
    saradc_add_channel(ADC_CHANNEL_TEST1 | REFERENCE_VOLTAGE_CHANNEL, SARADC_PULLUP_DIS);
    saradc_kick_start();
}

void saradc_general_ch_get_value_test_1(void)
{
    uint16_t vbg, bat_adc_val, bat_voltage;
    bat_adc_val = saradc_get_channel_value(ADC_CHANNEL_TEST1);
    vbg = saradc_get_channel_value(REFERENCE_VOLTAGE_CHANNEL);
    if (vbg) {
        bat_voltage = bat_adc_val * REFERENCE_VOLTAGE_VALUE / vbg;
        printf("test_1 bat_voltage = %dmv, bat_adc_val = %d\n", bat_voltage, bat_adc_val);
    }
}

void saradc_general_ch_user_proc(void)
{
    saradc_general_ch_get_value_test_0();
    saradc_general_ch_get_value_test_1();
}


void saradc_general_ch_proc(void)
{
    if (saradc_get_flag(SARADC_IT_FINISHED)) {
        saradc_clear_flag(SARADC_IT_FINISHED);
        saradc_get_result();
        saradc_kick_start();
    }
}
#endif  //SARADC_GENERAL_EN
