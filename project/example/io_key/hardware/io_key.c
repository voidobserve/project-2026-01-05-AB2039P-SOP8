#include "include.h"
#include "driver_gpio.h"
#include "io_key.h"

#define IOKEY_PORT          GPIOB_REG
#define IOKEY_PIN           GPIO_PIN_6

void io_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = IOKEY_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IOKEY_PORT, &gpio_init_structure);
}

void io_key_example(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, 40)) {
        if (gpio_read_bit(IOKEY_PORT, IOKEY_PIN) == SET) {
            printf("key is up\n");
        } else if (gpio_read_bit(IOKEY_PORT, IOKEY_PIN) == RESET) {
            printf("key is down\n");
        }
        delay_cnt = tick_get();
    }
}
