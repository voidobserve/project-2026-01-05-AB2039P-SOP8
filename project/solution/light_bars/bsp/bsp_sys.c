#include "include.h"

xcfg_cb_t xcfg_cb;
sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
u16 tmr5ms_cnt;

void sys_ram_info_dump(void)
{
    extern u32 __comm_size;
    extern u32 __bss_size;

    extern u32 __sram_vma;
    extern u32 __sram_ram_size;

    u32 sram_free  = (u32)&__sram_ram_size - (u32)&__comm_size - (u32)&__bss_size;

    printf("Ram_info:\n");
    printf("sram: remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", sram_free,  (u32)&__sram_ram_size,  (u32)&__sram_vma, (u32)&__sram_vma + (u32)&__sram_ram_size);

}


//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_callback(void)
{
    tmr5ms_cnt++;

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0) {
        lowpwr_tout_ticks();
    }

    //500ms timer process
    if ((tmr5ms_cnt % 100) == 0) {
        msg_enqueue(MSG_SYS_500MS);
    }

    //1s timer process
    if ((tmr5ms_cnt % 200) == 0) {
        msg_enqueue(MSG_SYS_1S);
    }
}

AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
#if SYS_SLEEP_EN
    sys_cb.sleep_time = -1L;
    sys_cb.sleep_delay = -1L;

    if (SYS_SLEEP_TIME != 0) {
        sys_cb.sleep_time = (u32)SYS_SLEEP_TIME * 10;   //100ms为单位
        sys_cb.sleep_delay = sys_cb.sleep_time;
    }
#endif

#if SYS_OFF_TIME
    sys_cb.pwroff_time = (u32)SYS_OFF_TIME * 10;    //100ms为单位
    sys_cb.pwroff_delay = sys_cb.pwroff_time;
#endif


    if(!xcfg_cb.ft_osc_cap_en || (xcfg_cb.osci_cap == 0 && xcfg_cb.osco_cap == 0)) {        //没有过产测时，使用自定义OSC电容
        xcfg_cb.osci_cap = xcfg_cb.uosci_cap;
        xcfg_cb.osco_cap = xcfg_cb.uosco_cap;
        xcfg_cb.osc_both_cap = xcfg_cb.uosc_both_cap;
    }


    msg_queue_init();
}


AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    uint32_t pmu_cfg;

#if BSP_UART_DEBUG_EN
    GPIOBDE = GPIO_PIN_GET(BSP_UART_DEBUG_EN);             //打印口保持数字IO，其他IO默认先设置成模拟IO，防止漏电
#else 
    GPIOBDE = 0;
#endif

    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    pmu_cfg = (BUCK_MODE_EN * PMU_CFG_BUCK_EN) | PMU_CFG_VBAT;

    bsp_var_init();

    // power init
    pmu_init(pmu_cfg);

    // clock init
    sys_clk_set(SYS_CLK_SEL);

    bsp_param_init();

    //晶振配置
    xosc_init();

    /// enable user timer
    sys_set_tmr_enable(1, 1);

    printf("%s\n",__func__);

    ble_app_env_init();

#if BSP_IR_EN
    bsp_irrx_decode_init();
#endif // BSP_IR_EN

#if BSP_LEDC_EN
    bsp_ws2815_ledc_init(LEDC_RGB);
#endif // BSP_IR_EN

#if BSP_MIC_EN
    saradc_mic_ch_init();
#endif

    func_cb.sta = FUNC_BT;
}

void bsp_periph_init(void)
{
#if BSP_UART_DEBUG_EN
    bsp_uart_debug_init();
#endif

#if BSP_IR_EN
    bsp_irrx_decode_init();
#endif // BSP_IR_EN

#if BSP_LEDC_EN
    bsp_ws2815_ledc_init(LEDC_RGB);
#endif // BSP_IR_EN

#if BSP_MIC_EN
    saradc_mic_ch_init();
#endif

#if BSP_TSEN_EN
    bsp_tsensor_init();
#endif // BSP_TSEN_EN
}

