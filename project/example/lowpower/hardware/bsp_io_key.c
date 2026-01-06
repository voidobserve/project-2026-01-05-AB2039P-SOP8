#include "include.h"
#include "bsp_io_key.h"

void bsp_io_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_5;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(GPIOB_REG, &gpio_init_structure);

    wko_io_config(ENABLE, DISABLE, ENABLE);
}

u8 bsp_get_io_key_id(void)
{
   if (wko_io_read_bit() == RESET) {
       return KEY_ID_PP;
   } else if (gpio_read_bit(GPIOB_REG, GPIO_PIN_5) == RESET) {
       return KEY_ID_K1;
   }

   return KEY_ID_NO;
}
