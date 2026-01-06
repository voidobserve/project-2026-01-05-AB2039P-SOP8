#include "include.h"
#include "driver_pwm.h"
#include "driver_gpio.h"
#include "pwm.h"

#define TIMERP_PWM0_PORT              GPIOB_REG
#define TIMERP_PWM0_PIN               GPIO_PIN_6

#define TIMERP_PWM1_PORT              GPIOB_REG
#define TIMERP_PWM1_PIN               GPIO_PIN_7

AT(.com_text.isr)
void pwm_isq(void)
{
    if (tmrp_get_flag(TMRP, TMRP_TIE) != RESET) {
        tmrp_clear_flag(TMRP, TMRP_TIE);
    }
}

void pwm_init(void)
{
    gpio_init_typedef gpio_init_structure;
    tmrp_base_init_typedef tmrp_base_init_struct;

    clk_gate2_cmd(CLK_GATE2_TMRP, CLK_EN);

    /*****************************初始化配置**********************************/
    gpio_init_structure.gpio_pin = TIMERP_PWM0_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(TIMERP_PWM0_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = TIMERP_PWM1_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(TIMERP_PWM1_PORT, &gpio_init_structure);

    gpio_func_mapping_config(TIMERP_PWM0_PORT, TIMERP_PWM0_PIN, GPIO_CROSSBAR_OUT_PWM0);
    gpio_func_mapping_config(TIMERP_PWM1_PORT, TIMERP_PWM1_PIN, GPIO_CROSSBAR_OUT_PWM1);

    tmrp_base_init_struct.clock_source = TMRP_XOSC_CLKDIV24;    // 1MHz
    tmrp_base_init_struct.clock_div = TMRP_CLOCK_DIV_4;         // div 4,  4us
    tmrp_base_init_struct.period = -1;                          // pwm 频率1，0
    tmrp_base_init_struct.g2_period = -1;                       // pwm 频率2，0
    tmrp_base_init_struct.tmr_mode = TMRP_TMR_MODE;
    tmrp_base_init_struct.tick_mode = TMRP_TICK_MODE_UNUSED;
    tmrp_base_init_struct.pwm_num = TMRP_PWM_UNUSED;
    tmrp_base_init_struct.pwm_inv = TMRP_PWM_INV_UNUSED;
    tmrp_base_init_struct.start_duty0 = -1;                     // duty  0
    tmrp_base_init_struct.start_duty1 = -1;                     // duty  0
    tmrp_base_init_struct.dz_en = DISABLE;                      // 死区, 0
    tmrp_base_init_struct.dzmode = TMRP_DZMODE_0;
    tmrp_base_init_struct.dzlen = 0;

    tmrp_base_init(TMRP, &tmrp_base_init_struct);
    tmrp_pic_config(TMRP, pwm_isq, 0, TMRP_TIE, ENABLE);
    tmrp_cmd(TMRP, ENABLE);


    /*****************************随用随开**********************************/
    tmrp_set_period(TMRP, 100-1);          //period 400us

    tmrp_open_pwm(TMRP, TMRP_PWM0, TMRP_PWM_INV_UNUSED);
    tmrp_set_duty(TMRP, TMRP_PWM0, 50-1);  //duty 50%

    tmrp_open_pwm(TMRP, TMRP_PWM1, TMRP_PWM_INV_UNUSED);
    tmrp_set_duty(TMRP, TMRP_PWM1, 25-1);  //duty 25%  
}

void pwm_example(void)
{
    //测试，15s后关闭PWM
    static bool test_flag = 1;
    static uint32_t tick = -1;
    if (tick_check_expire(tick, 15000) && test_flag) {
        test_flag = 0;
        tmrp_close_pwm(TMRP, TMRP_PWM0 | TMRP_PWM1);
        printf("*****close pwm*****\n");
    }
}

