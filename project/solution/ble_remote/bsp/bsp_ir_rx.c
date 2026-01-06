#include "include.h"
#include "bsp_ir_rx.h"
#include "driver_ir.h"
#include "driver_gpio.h"

#if (BSP_IR_RX_EN)

static uint8_t irrx_flag = 0; //0:nothing; 1:rx done; 2: key release;
static int16_t repeat_cnt = -1;
static uint32_t irrx_data = 0;

AT(.com_text.irq)
static void bsp_irrx_irq_handler(void)
{
    if (irrx_get_flag(IRRX_FLAG_RXPND)) {
        irrx_clear_flag(IRRX_FLAG_RXPND);
        irrx_flag = 1;
        irrx_data = irrx_get_data();
    }

    if (irrx_get_flag(IRRX_FLAG_KEYRELS)) {
        irrx_clear_flag(IRRX_FLAG_KEYRELS);
        irrx_flag = 2;
        irrx_data = 0;
    }
}

static void bsp_irrx_io_init(gpio_typedef* gpiox, uint32_t gpio_pin)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = gpio_pin;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(gpiox, &gpio_init_structure);

    gpio_func_mapping_config(gpiox, gpio_pin, GPIO_CROSSBAR_IN_IRRX);
}

void bsp_irrx_init(gpio_typedef* gpiox, uint32_t gpio_pin)
{
    irrx_param_t irrx_param;

    clk_gate1_cmd(CLK_GATE1_IRRX, CLK_EN);
    clk_irrx_clk_set(CLK_IR_CLK_X24M_CLKDIV24);

    bsp_irrx_io_init(gpiox, gpio_pin);

    irrx_param.wakup_en = IRRX_WAKUP_SLEEP_ENABLE;
    irrx_param.data_format = IRRX_DATA_SELECT_32BIT;

    irrx_param.filter_en = IRRX_FILTER_ENABLE;
    irrx_param.filter_srcs = IRRX_FILTER_SRCS_RXIN;
    irrx_param.filter_len = 3;

    irrx_init(&irrx_param);

    irrx_pic_config(bsp_irrx_irq_handler, 0, IRRX_FLAG_RXPND, ENABLE);

    irrx_cmd(ENABLE);
}

void bsp_irrx_proc(void)
{
    static uint32_t tick = -1;
    if (irrx_flag == 1) {
        // key down
        printf("irrx key down, irrx_data:0x%x\n", irrx_data);
        irrx_flag = 0;
        repeat_cnt = 0;
        tick = tick_get();
    } else if (irrx_flag == 2) {
        //key up
        printf("irrx key up\n");
        irrx_flag = 0;
        repeat_cnt = -1;
    }

    if ((repeat_cnt != -1) && tick_check_expire(tick, 110)) {
        //key hold
        tick = tick_get();
        repeat_cnt++;
        printf("repeat_cnt:0x%x\n", repeat_cnt);
    }
}

#endif //#if (BSP_IR_RX_EN)
