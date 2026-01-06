#ifndef _T_SEN_H
#define _T_SEN_H

#define REFERENCE_VOLTAGE_CHANNEL       ADC_CHANNEL_BG
#define REFERENCE_VOLTAGE_VALUE         pmu_get_vbg_mv()


void temperature_sensor_init(void);
void temperature_sensor_example(void);

#endif // _T_SEN_H
