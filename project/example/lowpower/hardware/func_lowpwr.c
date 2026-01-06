#include "include.h"
#include "driver_lowpwr.h"
#include "driver_gpio.h"
#include "func_lowpwr.h"
#if RTC_WK_EXAMPLE_EN
#include "driver_clk.h"
#include "driver_rtc.h"
#endif

extern void bsp_periph_init(void);
extern void user_set_sleep_stat(bool stat);

#define IO_KEY_WK0_EN               1

#if RTC_WK_EXAMPLE_EN
AT(.com_rodata.isr)
const char alarm_tip[] = "\n----------------->> alarm <<--------------------\n\n";

AT(.com_text.isr)
static void rtc_isr(void)
{
    if ((rtc_get_flag(RTC_IT_ALM) != RESET) || (rtc_get_overflow_flag(RTC_IT_ALM) != RESET)) {
        rtc_clear_flag(RTC_IT_ALM);
        printf(alarm_tip);
    }
}
#endif

void bsp_gpio_wakeup_config(uint8_t lp_type)
{
    lowpwr_wakeup_typedef config;
    gpio_edge_cap_typedef* gpio_cfg = &config.gpio_cfg;

    config.lp_type = lp_type;
    config.source = WK_LP_GPIO ;
    gpio_cfg->edge = WK_EDGE_FALLING;
    gpio_cfg->filter = 1;
    gpio_cfg->gpiox = GPIOB_REG;
    gpio_cfg->gpio_pin = GPIO_PIN_6;
    gpio_cfg->gpio_pupd = GPIO_PUPD_PU10K;
    lowpwr_gpio_wakeup_config(&config);

#if 0
    gpio_cfg->gpiox = GPIOB_REG;
    gpio_cfg->gpio_pin = GPIO_PIN_1;
    gpio_cfg->gpio_pupd = GPIO_PUPD_PU10K;
    gpio_cfg->wkpinmap = GPIO_WK7MAP;                  //gpio wk src 7
    gpio_cfg->wkpinmap_int = GPIO_WK7MAP_INT7_G1_PB1;  //PB1
    lowpwr_gpio_wakeup_config_custom(&config);

    gpio_cfg->gpiox = GPIOB_REG;
    gpio_cfg->gpio_pin = GPIO_PIN_2;
    gpio_cfg->gpio_pupd = GPIO_PUPD_PU10K;
    gpio_cfg->wkpinmap = GPIO_WK3MAP;                  //gpio wk src 3
    gpio_cfg->wkpinmap_int = GPIO_WK3MAP_INT3_G3_PB2;  //PB2
    lowpwr_gpio_wakeup_config_custom(&config);
#endif

#if IO_KEY_WK0_EN
    config.source |= WK_LP_WK0;
    lowpwr_wk0_wakeup_config(&config);
#endif
}

#if SLEEP_SFR_SAVE
/*
    Users save the registers that need to be backed up.
    Registers that are not backed up will lose their values after they enter deep sleep
*/
typedef struct {
    u32 uart0baud;
    u16 uart0con;
} lowpwr_sfr_user_backup_t;

lowpwr_sfr_user_backup_t sfr_user_backup;

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_save(void)
{
	sfr_user_backup.uart0baud = UART0BAUD;
	sfr_user_backup.uart0con = UART0CON;
}

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_resume(void)
{
    UART0BAUD = sfr_user_backup.uart0baud;
    UART0CON = sfr_user_backup.uart0con;
    UART0CPND = BIT(17) | BIT(16);                  //CLR Pending
}
#endif

void lowpwr_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

#if RTC_WK_EXAMPLE_EN
    config.source = WK_LP_ALARM;
#endif
    config.lp_type = LP_TYPE_SLEEP;

    lowpwr_wakeup_config(&config);

    bsp_gpio_wakeup_config(LP_TYPE_SLEEP);

#if RTC_WK_EXAMPLE_EN
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_SNF_RC_RTC); //enable rtc clock
    u32 snf_rc2m_clk = rc2m_clk_nhz_get(0); //get rc2m actual frequency
    rtc_prescale_set((snf_rc2m_clk/64)-1); //Config predivision factor for 1Hz counter
    rtc_pic_config(rtc_isr, 0, RTC_IT_ALM, ENABLE);
    rtc_set_alarm(rtc_get_counter()+4); //Set the alarm to wake up after 5s
#endif
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t sysclk;
    u8 pg_de;
    u16 pb_de;

    printf("sleep enter\n");

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    //io analog input
    pb_de = GPIOBDE;
    pg_de = GPIOGDE;
    GPIOBDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    lowpwr_sleep_wakeup_config();

    lowpwr_sleep_proc();    //!!go to sleep, the program stops on this line!!!

    //Reinitialize all peripherals, because all register configurations are lost after low power
    bsp_periph_init();

    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_get_wakeup_source();
    lowpwr_clr_wakeup_pending();

    sys_clk_set(sysclk);

    user_set_sleep_stat(0); //example, update sleep status

#if RTC_WK_EXAMPLE_EN
    //fixed the bug with no alarm pending
    if (rtc_get_overflow_flag(RTC_IT_ALM)) {
        rtc_isr();
    }
#endif //RTC_WK_EXAMPLE_EN

    printf("sleep exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (is_sleep()) {
        sfunc_sleep();
        return true;
    } else {
        return false;
    }
}

void lowpwr_pwroff_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

#if RTC_WK_EXAMPLE_EN
    config.source = WK_LP_WK0 | WK_LP_ALARM;
#else
    config.source = WK_LP_WK0;
#endif
    config.lp_type = LP_TYPE_POWOFF;

    lowpwr_wakeup_config(&config);

    bsp_gpio_wakeup_config(LP_TYPE_POWOFF);

#if RTC_WK_EXAMPLE_EN
    clk_clk32k_rtc_set(CLK_CLK32K_RTC_SNF_RC_RTC); //enable rtc clock
    u32 snf_rc2m_clk = rc2m_clk_nhz_get(0); //get rc2m actual frequency
    rtc_prescale_set((snf_rc2m_clk/64)-1); //Config predivision factor for 1Hz counter
    rtc_set_alarm(rtc_get_counter()+4); //Set the alarm to wake up after 5s
#endif
}

void func_pwroff(void)
{
    printf("%s\n", __func__);

    GPIOBDE = 0;
    GPIOBPU = 0;
    
    lowpwr_pwroff_wakeup_config();

#if (SYS_PWROFF_MODE  == POWEROFF_MODE1)
    lowpwr_pwroff_mode1_do();
#else
    lowpwr_pwroff_mode2_do();
#endif
}
