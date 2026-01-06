#include "include.h"
#include "driver_gpio.h"
#include "driver_pwm.h"

#if BSP_PWM_EN

#define BSP_PEN_PWM_PORT                GPIOB_REG
#define BSP_PEN_PWM_A_PIN               GPIO_PIN_8
#define BSP_PEN_PWM_B_PIN               GPIO_PIN_9

// api_xxx.h
void pll_ext_init(void);


void bsp_pwm_init(void)
{
    gpio_init_typedef gpio_init_struct;
    tmrp_base_init_typedef tmrp_base_init_struct;

    /* pll: 448.125MHz  plldiv4 -> clkout_div7 -> TMRP */
    pll_ext_init();
    clk_clkout_set(CLK_CLKOUT_PLLDIV4, 7 - 1, CLK_EN);
    clk_gate2_cmd(CLK_GATE2_TMRP, CLK_EN);

    gpio_init_struct.gpio_pin  = BSP_PEN_PWM_A_PIN | BSP_PEN_PWM_B_PIN;
    gpio_init_struct.gpio_dir  = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_fen  = GPIO_FEN_PER;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_drv  = GPIO_DRV_I_6MA;
    gpio_init(BSP_PEN_PWM_PORT, &gpio_init_struct);

    gpio_func_mapping_config(BSP_PEN_PWM_PORT, BSP_PEN_PWM_A_PIN, GPIO_CROSSBAR_OUT_PWM0);
    gpio_func_mapping_config(BSP_PEN_PWM_PORT, BSP_PEN_PWM_B_PIN, GPIO_CROSSBAR_OUT_PWM3);

    tmrp_base_init_struct.clock_source = TMRP_CLKOUT_PINP;
    tmrp_base_init_struct.clock_div    = TMRP_CLOCK_DIV_1;
    tmrp_base_init_struct.period       = 33 - 1;
    tmrp_base_init_struct.g2_period    = 34 - 1;
    tmrp_base_init_struct.tmr_mode     = TMRP_TMR_MODE_G2;
    tmrp_base_init_struct.tick_mode    = TMRP_TICK_MODE_UNUSED;
    tmrp_base_init_struct.pwm_num      = TMRP_PWM0 | TMRP_PWM3;
    tmrp_base_init_struct.pwm_inv      = TMRP_PWM_INV_UNUSED;
    tmrp_base_init_struct.start_duty0  = 16 - 1;
    tmrp_base_init_struct.start_duty3  = 17 - 1;
    tmrp_base_init_struct.dz_en        = DISABLE;
    tmrp_base_init_struct.dzmode       = TMRP_DZMODE_0;
    tmrp_base_init_struct.dzlen        = 0;

    tmrp_base_init(TMRP, &tmrp_base_init_struct);
    tmrp_cmd(TMRP, ENABLE);
}

void bsp_pwm_process(void)
{
    /* Do nothing */
}

#endif // BSP_PWM_EN
