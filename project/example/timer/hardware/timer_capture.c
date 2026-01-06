#include "include.h"
#include "driver_tmr.h"
#include "driver_gpio.h"
#include "timer_capture.h"

#define TIMER_CAPTURE_PORT              GPIOB_REG
#define TIMER_CAPTURE_PIN               GPIO_PIN_5

volatile u32 tmr_capture_overflow_cnt = 0;

AT(.com_rodata.capture)
const char capture_info[] = "capture! interval: %d us\n";
AT(.com_rodata.capture)
const char overflow_info[] = "overflow!\n";

AT(.com_text.capture_interval)
u32  tmr_get_capture_interval(u32 overflow_cnt,u32 clk_src, u32 clk_div)
{
    if(clk_src !=TMR_XOSC24M_CLK&&clk_src !=TMR_XOSC_CLKDIV24){
       printf(capture_info, 11);
       return 0;
    }
    if(clk_div ==TMR_CLOCK_DIV_UNUSED){
         return 0;
    }
    u8 div_val=0;
    if(clk_div==TMR_CLOCK_DIV_1){
        div_val=1;
    }else  if(clk_div==TMR_CLOCK_DIV_2){
        div_val=2;
    }
    else  if(clk_div==TMR_CLOCK_DIV_4){
        div_val=4;
    }
    else  if(clk_div==TMR_CLOCK_DIV_8){
        div_val=8;
    }
     else  if(clk_div==TMR_CLOCK_DIV_16){
        div_val=16;
    }
    else  if(clk_div==TMR_CLOCK_DIV_32){
        div_val=32;
    }
    else  if(clk_div==TMR_CLOCK_DIV_64){
        div_val=64;
    }

    u32 interval=0;
    if(clk_src ==TMR_XOSC24M_CLK){
        interval = overflow_cnt * (tmr_get_period(TMR3) + 1) + tmr_get_counter(TMR3);
        interval = interval/(24/div_val);
    }

    if(clk_src ==TMR_XOSC_CLKDIV24){
        interval = overflow_cnt * (tmr_get_period(TMR3) + 1) + tmr_get_counter(TMR3);
        printf(capture_info, interval);
        interval = interval/(1/div_val);
    }
    return interval;
}




AT(.com_text.isr)
void timer3_cap_handle(void)
{
    u32 interval;

    if (tmr_get_flag(TMR3, TMR_IT_CAPTURE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_CAPTURE);
        interval=tmr_get_capture_interval(tmr_capture_overflow_cnt,TMR_XOSC24M_CLK,TMR_CLOCK_DIV_4);
        tmr_capture_overflow_cnt = 0;
        tmr_set_counter(TMR3, 0);
        printf(capture_info, interval);
    }

    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        tmr_capture_overflow_cnt++;
        printf(overflow_info);
    }
}

void timer3_capture_init(u32 period)
{
    gpio_init_typedef gpio_init_structure;
    tmr_base_init_typedef tmr_base_init_struct;

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    gpio_init_structure.gpio_pin = TIMER_CAPTURE_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PD10K;
    gpio_init(TIMER_CAPTURE_PORT, &gpio_init_structure);

    gpio_func_mapping_config(TIMER_CAPTURE_PORT, TIMER_CAPTURE_PIN, GPIO_CROSSBAR_IN_TMR3CAP);

    tmr_base_init_struct.tick_en = DISABLE;
    tmr_base_init_struct.period = period;
    tmr_base_init_struct.tmr_mode = TMR_CAP_RISING_EDGE;
    tmr_base_init_struct.clock_source = TMR_XOSC24M_CLK;
    tmr_base_init_struct.clock_div = TMR_CLOCK_DIV_4;
    tmr_base_init(TMR3, &tmr_base_init_struct);
    tmr_pic_config(TMR3, timer3_cap_handle, 0, TMR_IT_CAPTURE | TMR_IT_UPDATE, ENABLE);

    tmr_cmd(TMR3, ENABLE);
}

void timer3_capture_example(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, 1000)) {
        printf("---> system is running.\n");
        delay_cnt = tick_get();
    }
}
