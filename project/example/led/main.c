#include "include.h"
#include "driver_gpio.h"


int main(void)
{
    WDT_DIS();

    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_0 | GPIO_PIN_8;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(GPIOB_REG, &gpio_init_structure);

    gpio_set_bits(GPIOB_REG, GPIO_PIN_0);
    gpio_reset_bits(GPIOB_REG, GPIO_PIN_8);

    while(1){
        gpio_toggle_bits(GPIOB_REG, GPIO_PIN_0 | GPIO_PIN_8);
        delay_ms(500);
    }

    return 0;
}
