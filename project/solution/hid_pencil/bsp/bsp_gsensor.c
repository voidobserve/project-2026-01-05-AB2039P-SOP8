#include "include.h"
#include "driver_gpio.h"

#if BSP_GSENSOR_DETECT_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf("[BSP] "); \
                                printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif


static struct {
    uint32_t tick;
    uint8_t  state;
} bsp_gsensor_cb AT(.bram_text.bsp.gsensor);


void bsp_gsensor_init(void)
{
    gpio_init_typedef gpio_init_struct;

    memset(&bsp_gsensor_cb, 0x00, sizeof(bsp_gsensor_cb));

    gpio_init_struct.gpio_pin  = BSP_GSENSOR_PIN;
    gpio_init_struct.gpio_dir  = GPIO_DIR_INPUT;
    gpio_init_struct.gpio_pupd = GPIO_PUPD_PU200K;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen  = GPIO_FEN_GPIO;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init(BSP_GSENSOR_PORT, &gpio_init_struct);
}

void bsp_gsensor_process(void)
{
    do {
        if (false == tick_check_expire(bsp_gsensor_cb.tick, 100)) {
            break;
        }

        bsp_gsensor_cb.tick = tick_get();

        uint8_t state = bsp_gsensor_state_get();

        if (state == bsp_gsensor_cb.state) {
            break;
        }

        TRACE("g-sensor change: %d\n", state);
        bsp_gsensor_cb.state = state;

    } while (0);
}

#endif // BSP_GSENSOR_DETECT_EN
