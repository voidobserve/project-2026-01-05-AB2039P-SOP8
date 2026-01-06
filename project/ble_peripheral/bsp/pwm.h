#ifndef _PWM_H
#define _PWM_H


void pwm_init(void);

void light_module_set_bright(u16 p_liangud);
void light_module_10ms_function(void);
void pwm_driver_10ms_isr(void);
void light_module_ble_event(u8 *buffer, u16 len);
void light_mode_event(void);
void light_module_check_save_param_event(void);

#endif // _TIMER_LED_H
