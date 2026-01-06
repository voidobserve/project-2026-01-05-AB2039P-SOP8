#include "include.h"
#include "driver_lowpwr.h"
#include "usb_com.h"
#include "usb_hid.h"


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

AT(.com_text.dev.detect)
void usb_detect(void)
{
    u8 usb_sta = usbchk_host_connect();

    if (usb_sta == USB_HOST_CONNECTED) {
        if (dev_online_filter(DEV_USBPC)) {
            msg_enqueue(MSG_SYS_PC_INSERT);
        }
    } else {
    /*dongle从USB口取电，拔出即掉电，不做拔出检测（resume信号可能出现主机不ending或者很晚才ending导致设备认为拔出）*/

//        if (dev_offline_filter(DEV_USBPC)) {
//            sys_cb.usb_is_active = 0;
//            msg_enqueue(MSG_SYS_PC_REMOVE);
//        }
    }
}

//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_callback(void)
{
    tmr5ms_cnt++;

#if BSP_KEY_EN
    bsp_key_scan(0);
#endif

#if USB_EN
    usb_detect();
#endif // USB_EN

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0) {

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
    uint32_t pmu_cfg = 0;

#if BSP_UART_DEBUG_EN
    GPIOBDE = GPIO_PIN_GET(BSP_UART_DEBUG_EN);             //打印口保持数字IO，其他IO默认先设置成模拟IO，防止漏电
#else 
    GPIOBDE = 0;
#endif

#if BSP_UART_DEBUG_EN
    bsp_uart_debug_init();
#endif

    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    pmu_cfg = (BUCK_MODE_EN * PMU_CFG_BUCK_EN) | (PMU_CFG_VBAT);

    bsp_var_init();

#if BSP_KEY_EN
    bsp_key_init();
#endif

    // power init
    pmu_init(pmu_cfg);

    // clock init
    sys_clk_set(SYS_CLK_SEL);

    bsp_param_init();

    xosc_init();

    // enable user timer callback
    sys_set_tmr_enable(1, 1);


#if USB_EN
    dev_init(40, 40);
    usb_device_init();
#endif // USB_EN

    printf("%s\n",__func__);

    func_cb.sta = FUNC_BT;
}

void bsp_periph_init(void)
{
#if BSP_UART_DEBUG_EN
    bsp_uart_debug_init();
#endif

}
