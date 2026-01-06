#include "include.h"
#include "bsp_uart_debug.h"
#include "func_lowpwr.h"
#include "bsp_io_key.h"

static bool user_sleep_stat = 0;
static uint8_t pwron_cnt = 0;
static uint8_t pwroff_cnt = 0;

void bsp_periph_init(void)
{
    bsp_uart_debug_init();
    bsp_io_key_init();
}

void user_set_sleep_stat(bool stat)
{
    user_sleep_stat = stat;
}

bool user_get_sleep_stat(void)
{
    return user_sleep_stat;
}

void user_poweron_check(void)
{
    while (1) {
        if (bsp_get_io_key_id() == KEY_ID_PP) {
            delay_5ms(100);
            pwron_cnt++;
            printf("pwron_cnt:%d\n", pwron_cnt);
            if (pwron_cnt >= 4) {
                printf("power on!\n");
                break;
            }
        } else {
            pwron_cnt = 0;
            func_pwroff();
        }
    }
}

void user_poweroff_check(void)
{
    if ((bsp_get_io_key_id() == KEY_ID_PP) && (pwron_cnt == 0)) {
        delay_5ms(100);
        pwroff_cnt++;
        printf("pwroff_cnt:%d\n", pwroff_cnt);
    } else if (bsp_get_io_key_id() == KEY_ID_NO) {  //按键抬起
        pwron_cnt = 0;
        pwroff_cnt = 0;
    }

    if (pwroff_cnt == 6) {
        func_pwroff();
    }
}

void user_sleep_check(void)
{
    if (bsp_get_io_key_id() == KEY_ID_K1) {
        printf("KEY_ID_K1\n");
        delay_5ms(50);
        user_set_sleep_stat(1);
    }
}

int main(void)
{
    u32 rst_reason = sys_rst_init(0);

    bsp_periph_init();

    sys_rst_dump(rst_reason);

    printf("--->lowpower example\n");

    printf("!!!!!!!!!!!!!!!!!\n");
    printf("--->WK0:long press to power on/power off\n");
    printf("--->PB5:short press to enter sleep\n");
    printf("--->PB6:short press to exit sleep\n");
    printf("!!!!!!!!!!!!!!!!!\n");

#if SYS_PWROFF_TEST
    user_poweron_check();   //长按开机
#endif

    pmu_init(BUCK_MODE_EN);  // power init

    while (1) {
        WDT_CLR();  //退出sleep会自动打开WDT，该例程需持续清狗

#if SYS_PWROFF_TEST
        user_poweroff_check();  //长按关机
#endif

#if SYS_SLEEP_TEST
        user_sleep_check();  //模拟进入sleep

        sleep_process(user_get_sleep_stat);  //sleep处理
#endif
    }

    return 0;
}
