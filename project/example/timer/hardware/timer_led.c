#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "timer_led.h"

#define TIMER_LED_INTERRUPT_EN      1
#define TIMER_LED_PORT              GPIOB_REG
#define TIMER_LED_PIN               GPIO_PIN_5

AT(.com_text.isr)
void timer3_isq(void)
{
    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        gpio_toggle_bits(TIMER_LED_PORT, TIMER_LED_PIN);
    }
}

void timer3_led_init(void)
{
    gpio_init_typedef gpio_init_structure;
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    gpio_init_structure.gpio_pin = TIMER_LED_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(TIMER_LED_PORT, &gpio_init_structure);

    tmr_base_init_struct.tick_en = DISABLE;
    tmr_base_init_struct.period = 125000 - 1;                 // 0.5s
    tmr_base_init_struct.tmr_mode = TMR_TIMER_MODE;           // 计数
    tmr_base_init_struct.clock_div = TMR_CLOCK_DIV_4;         // 4分频
    tmr_base_init_struct.clock_source = TMR_XOSC_CLKDIV24;    // 1MHz
    tmr_base_init(TMR3, &tmr_base_init_struct);

#if TIMER_LED_INTERRUPT_EN
    tmr_pic_config(TMR3, timer3_isq, 0, TMR_IT_UPDATE, ENABLE);
#endif // TIMER_LED_INTERRUPT_EN

    tmr_cmd(TMR3, ENABLE);
}

void timer3_led_example(void)
{
#if TIMER_LED_INTERRUPT_EN == 0
    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        gpio_toggle_bits(TIMER_LED_PORT, TIMER_LED_PIN);
    }
#endif // TIMER_LED_INTERRUPT_EN
}

