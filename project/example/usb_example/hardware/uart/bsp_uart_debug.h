#ifndef _BSP_UART_DEBUG_H
#define _BSP_UART_DEBUG_H

#include "include.h"

#define UART_DEBUG_BAUD                 1500000
#define UART_DEBUG_PORT_SEL             GPIOB_REG
#define UART_DEBUG_PIN_SEL              GPIO_PIN_5

void bsp_uart_debug_init(void);

#endif // _BSP_UART_DEBUG_H
