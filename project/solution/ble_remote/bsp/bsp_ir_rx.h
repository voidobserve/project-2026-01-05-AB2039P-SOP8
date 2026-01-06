#ifndef _BSP_IR_RX_H
#define _BSP_IR_RX_H
#include "include.h"
#include "driver_gpio.h"

void bsp_irrx_init(gpio_typedef* gpiox, uint32_t gpio_pin);
void bsp_irrx_proc(void);

#endif
