#include "include.h"
#include "driver_saradc.h"
#include "t_sen.h"


#define TSEN_CALIBRATION_DEBUG      0


/**
 * @brief ADC values and temperature mapping table.
 *        The actual error of the table is affected by TSEN sensitivity and SARADC accuracy. 
 *        And there may be slight differences between different chip individuals.
 */
#define T_STEP                      5
#define T_MIN                       -30
#define T_TABLE_SIZE                33
const u16 tsen_adc_map[T_TABLE_SIZE] = {
//  -30   -25   -20   -15   -10   -5     0    5
    721,  733,  746,  759,  772,  786,  805,  820,
//  10    15    20    25    30    35    40    45
    834,  848,  865,  873,  889,  902,  914,  924,
//  50    55    60    65    70    75    80    85
    947,  956,  965,  993,  1000, 1012, 1027, 1042,
//  90    95    100   105   110   115   120   125
    1057, 1070, 1085, 1094, 1118, 1132, 1144, 1162,
//  130
    1180,
};


void temperature_sensor_init(void)
{
    saradc_base_init_typedef saradc_base_init_struct;

    /* Enable Clock Gate and Select SARADC Clock Source */
    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_EN);
    clk_saradc_clk_set(CLK_SARADC_CLK_X24M_CLKDIV2);

    /* Initialize SARADC Base Register */
    saradc_base_init_struct.baud = 1500000;
    saradc_base_init_struct.mode = SARADC_MODE_NORMAL;
    saradc_base_init_struct.auto_analog = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_base_init_struct.auto_analog_io = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_init(&saradc_base_init_struct);

    saradc_cmd(ENABLE);
    
    saradc_add_channel(ADC_CHANNEL_TSEN | REFERENCE_VOLTAGE_CHANNEL, SARADC_PULLUP_DIS);
    saradc_kick_start();
}

/* 
  The time of blocking the system of this function depends on the sampling
  time and sampling rate and the number of mean filtering repeats.
 */
void temperature_sensor_example(void)
{
    static u32 delay_cnt;
    u32 reference_adc_value = 0, tsen_adc_value = 0;
    u16 tsen_voltage_value;
    int temperature;
    u8 i, loop = 5;

    /* The Interval between each TSEN Sampling */
    if (!tick_check_expire(delay_cnt, 500)) {
        return;
    }
    delay_cnt = tick_get();
    
    /* Average Filtering */
    for (i = 0; i < loop; i++) {
        saradc_kick_start();
        while (!saradc_get_flag(SARADC_IT_FINISHED));
        saradc_get_result();
        tsen_adc_value += saradc_get_channel_value(ADC_CHANNEL_TSEN);
        reference_adc_value += saradc_get_channel_value(REFERENCE_VOLTAGE_CHANNEL);
    }
    tsen_adc_value /= loop;
    reference_adc_value /= loop;

    /* Calc TSEN Voltage(mV) */
    tsen_voltage_value = REFERENCE_VOLTAGE_VALUE * tsen_adc_value / reference_adc_value;

#if TSEN_CALIBRATION_DEBUG
    printf("tsen_voltage_value: %d\n", tsen_voltage_value);
#else
    /* Convert to Temperature */
    for (i = 0; i < T_TABLE_SIZE; i++) {
        if (tsen_voltage_value < tsen_adc_map[i]) {
            break;
        }
    }

    if (i == 0) {
        temperature = -30;
    } else if (i == T_TABLE_SIZE) {
        temperature = 130;
    } else {
        if ((tsen_voltage_value - tsen_adc_map[i - 1]) < (tsen_adc_map[i] - tsen_voltage_value)) {
            temperature = T_MIN + (i - 1) * T_STEP;
        } else {
            temperature = T_MIN + i * T_STEP;
        }
    }

    printf("temperature: %d\n", temperature);
#endif // TSEN_CALIBRATION_DEBUG
}
