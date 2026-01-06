#include "include.h"
#include "driver_gpio.h"
#include "io_interrupt.h"


#define IOKEY_PORT          GPIOB_REG
#define IOKEY_PIN           GPIO_PIN_6

char info[] = "io_isr\n";

AT(.com_text.gpio_isr)
void gpio_isr_callback(void)
{
    gpio_edge_pending_clear();

    printf(info);
}

void io_interrupt_init(void)
{
    gpio_edge_cap_typedef config;

    config.edge = GPIO_EDGE_FALLING;
    config.gpiox = IOKEY_PORT;
    config.gpio_pin = IOKEY_PIN;

    gpio_edge_capture_config(&config);

    gpio_edge_pic_config(gpio_isr_callback, 0, ENABLE);
}
