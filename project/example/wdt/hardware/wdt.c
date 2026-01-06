#include "include.h"
#include "driver_wdt.h"
#include "driver_gpio.h"
#include "wdt.h"

#define WDT_LED_PORT                GPIOB_REG
#define WDT_LED_PIN                 GPIO_PIN_0
#define WDT_KEY_PORT                GPIOB_REG
#define WDT_KEY_PIN                 GPIO_PIN_7

static void wdt_led_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = WDT_LED_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;

    gpio_init(WDT_LED_PORT, &gpio_init_structure);
}

static void wdt_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = WDT_KEY_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(WDT_KEY_PORT, &gpio_init_structure);
}

static void wdt_test_init(void)
{
    wdt_time_select(WDT_TIME_4096MS);
    wdt_reset_cmd(ENABLE);
    wdt_cmd(ENABLE);
}

void wdt_run(void)
{
    u8 cnt = 0;
    wdt_test_init();
    wdt_led_init();
    wdt_key_init();

    while (1) {
        if (gpio_read_bit(WDT_KEY_PORT, WDT_KEY_PIN) == RESET) {
            printf("wdt clear\n");
            wdt_clear();
        }

        if(cnt == 5) {
            cnt = 0;
            gpio_toggle_bits(WDT_LED_PORT, WDT_LED_PIN);
        }
        cnt++;
        delay_ms(10);
    }

}
