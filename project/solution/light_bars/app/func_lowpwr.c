#include "include.h"
#include "driver_lowpwr.h"
#include "driver_gpio.h"
#include "ble_user_service.h"

extern u32 __pdn_comm_vma, __pdn_comm_lma, __pdn_comm_size;

#if SYS_SLEEP_EN
/*
    Users save the registers that need to be backed up.
    Registers that are not backed up will lose their values after they enter deep sleep
*/

typedef struct {
    u32 funcomcon[6];

    u32 uart0baud;
    u16 uart0con;
} lowpwr_sfr_user_backup_t;

lowpwr_sfr_user_backup_t sfr_user_backup;

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_save(void)
{
#if BSP_UART_DEBUG_EN
    sfr_user_backup.funcomcon[0] = FUNCOMCON0;
	sfr_user_backup.funcomcon[1] = FUNCOMCON1;
	sfr_user_backup.funcomcon[2] = FUNCOMCON4;
	sfr_user_backup.funcomcon[3] = FUNCOMCON5;
	sfr_user_backup.funcomcon[4] = FUNCOMCON6;
	sfr_user_backup.funcomcon[5] = FUNCIMCON0;

	sfr_user_backup.uart0baud = UART0BAUD;
	sfr_user_backup.uart0con = UART0CON;
#endif
}

AT(.com_sleep.lv2)
void lowpwr_sleep_sfr_user_resume(void)
{
#if BSP_UART_DEBUG_EN
    FUNCOMCON0 = sfr_user_backup.funcomcon[0];
    FUNCOMCON1 = sfr_user_backup.funcomcon[1];
	FUNCOMCON4 = sfr_user_backup.funcomcon[2];
	FUNCOMCON5 = sfr_user_backup.funcomcon[3];
	FUNCOMCON6 = sfr_user_backup.funcomcon[4];
    FUNCIMCON0 = sfr_user_backup.funcomcon[5];

    UART0BAUD = sfr_user_backup.uart0baud;
    UART0CON = sfr_user_backup.uart0con;
    UART0CPND = BIT(17) | BIT(16);                  //CLR Pending
#endif
}
#endif

AT(.com_text.sleep_tick)
void lowpwr_tout_ticks(void)
{
#if SYS_SLEEP_TIME
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0) {
        sys_cb.sleep_delay--;
    }
#endif

#if SYS_PWROFF_TIME
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0) {
        sys_cb.pwroff_delay--;
    }
#endif
}


#if SYS_SLEEP_EN
AT(.text.app.proc.sleep)
static bool lowpwr_can_enter_sleep(void)
{
    bool can_sleep = true;

    u16 msg = msg_dequeue();

    if(msg){
        switch(msg){
            case MSG_SYS_1S:
            case MSG_SYS_500MS:
                break;

            default:
                msg_enqueue(msg);
                can_sleep = false;
                break;
        }
    }

    return can_sleep;
}

static bool lowpwr_need_exit_sleep(void)
{
    if (lowpwr_is_wakeup_pending(WK_LP_BT)) {
        return true;
    }

    if (ble_proc_pending()) {
        return true;
    }

#if SYS_PWROFF_TIME
    if(sys_cb.pwroff_delay == 0) {
        return true;
    }
#endif

    return false;
}

AT(.com_sleep.sleep)
void sys_sleep_cb(void)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用com_sleep或com_text函数

    //此处关掉影响功耗的模块

    sys_cb.sleep_sta = LOWPWR_SLEEP_ENTER;
    lowpwr_sleep_proc();                  //enter sleep

    //唤醒后，恢复模块功能
}


void lowpwr_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = WK_LP_BT;

    config.lp_type = LP_TYPE_SLEEP;
    lowpwr_wakeup_config(&config);
}

AT(.com_sleep.lowpwr.sleep)
void sfunc_sleep_do(void)
{
    while (ble_is_sleep() && !sys_cb.sleep_prevent) {
        WDT_CLR();

        if(lowpwr_need_exit_sleep()){
            break;
        }

        ble_sleep_proc();
    }
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint8_t sysclk;
    u8 pg_de;
    u16 pb_de;

    printf("%s\n", __func__);

    sys_cb.sleep_sta = LOWPWR_SLEEP_PREPARE;

    ble_enter_sleep_proc();
    delay_5ms(1);
    ble_enter_sleep();

    sys_set_tmr_enable(0, 0);

    //io analog input
    pb_de = GPIOBDE;
    pg_de = GPIOGDE;
    //Retain the wakeup port configuration
    GPIOBDE &= (gpio_get_edge_pin() & 0xfff);
    //MCP FLASH
    GPIOGDE = 0x3F;

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    sys_cb.sleep_enter = true;

    sfunc_sleep_do();

    sys_clk_set(sysclk);

    //Reinitialize all peripherals, because all register configurations are lost after low power
    if(sys_cb.sleep_sta == LOWPWR_SLEEP_ENTER){
        bsp_periph_init();
    }

    printf("wakeup\n");

    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_clr_wakeup_pending();

    sys_set_tmr_enable(1, 1);

    ble_exit_sleep();
    ble_exit_sleep_proc();

    sys_cb.sleep_sta = LOWPWR_SLEEP_EXIT;
    sys_cb.sleep_prevent = false;
    sys_cb.sleep_enter = false;

    printf("sleep_exit\n");
}


AT(.text.lowpwr.deep_sleep_wk)
void lowpwr_deep_sleep_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

    config.lp_type = LP_TYPE_SLEEP;
    lowpwr_wakeup_config(&config);
}

/*
    Enter deep sleep, close the ble module, exit sleep, restart the ble module
*/
AT(.text.lowpwr.deep_sleep)
void sfunc_deep_sleep(void)
{
    uint32_t sysclk;
    u8 pg_de;
    u16 pb_de;

    printf("%s\n", __func__);

    sys_cb.sleep_sta = LOWPWR_SLEEP_PREPARE;

    btstack_deinit();

    sys_set_tmr_enable(0, 0);

    lowpwr_deep_sleep_wakeup_config();

     //io analog input
    pb_de = GPIOBDE;
    pg_de = GPIOGDE;
    //Retain the wakeup port configuration
    GPIOBDE &= PORTINTEN & 0xfff;
    //MCP FLASH
    GPIOGDE = 0x3F;

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    if(sys_cb.sleep_prevent == false){
        sys_cb.sleep_enter = true;
        lowpwr_sleep_proc();
    }

    sys_clk_set(sysclk);

    //Reinitialize all peripherals, because all register configurations are lost after low power
    if(sys_cb.sleep_sta == LOWPWR_SLEEP_ENTER){
        bsp_periph_init();
    }

    printf("wakeup deep\n");

    GPIOBDE = pb_de;
    GPIOGDE = pg_de;

    lowpwr_clr_wakeup_pending();

    sys_set_tmr_enable(1, 1);

    func_bt_init();

    sys_cb.sleep_sta = LOWPWR_SLEEP_EXIT;
    sys_cb.sleep_prevent = false;
    sys_cb.sleep_enter = false;

    lowpwr_sleep_wakeup_config();

    printf("sleep_exit_deep\n");
}

AT(.text.app.proc.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (sys_cb.sleep_delay == -1L) {
        return false;
    }

    if (is_sleep()) {
        if((sys_cb.sleep_delay == 0) && lowpwr_can_enter_sleep()) {
            sfunc_sleep();
            lowpwr_sleep_delay_reset();
            lowpwr_pwroff_delay_reset();
            return true;
        }
    } else {
        lowpwr_sleep_delay_reset();
    }

    return false;
}
#endif

void lowpwr_pwroff_wakeup_config(void)
{
    lowpwr_wakeup_typedef config;

    lowpwr_wakeup_disable();

    config.source = 0;

    config.lp_type = LP_TYPE_POWOFF;

    lowpwr_wakeup_config(&config);
}


AT(.text.lowpwr.pwroff)
void func_pwroff(void)
{
    printf("%s\n", __func__);

    GPIOBDE = 0;
    GPIOBPU = 0;

    lowpwr_pwroff_wakeup_config();

#if (BSP_IOKEY_EN && IO_KEY_SCAN_MODE)
    bsp_gpio_scan_mode_de_save();
#endif

    memcpy(&__pdn_comm_vma, &__pdn_comm_lma, (u32)&__pdn_comm_size);    //把代码加载到ram里面执行

#if SYS_PWROFF_MODE == PWROFF_MODE1
    lowpwr_pwroff_mode1_do();
#else
    lowpwr_pwroff_mode2_do();
#endif
}
