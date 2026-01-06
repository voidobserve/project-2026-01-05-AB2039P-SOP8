#ifndef _BSP_IR_TX_H
#define _BSP_IR_TX_H
#include "include.h"
#include "driver_gpio.h"

void bsp_irtx_init(gpio_typedef* gpiox, uint32_t gpio_pin);
void bsp_itx_send_data(uint8_t address, uint8_t cmd, bool repeat);

#endif
