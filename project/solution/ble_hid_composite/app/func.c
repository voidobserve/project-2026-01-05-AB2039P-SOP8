#include "include.h"
#include "app_hid_event.h"

func_cb_t func_cb AT(.buf.func_cb);

AT(.text.app.proc.func)
void func_process(void)
{
    WDT_CLR();

#if BSP_VBAT_DETECT_EN
    bsp_vbat_process();
#endif

#if BSP_TSENSOR_EN
    bsp_tsensor_process();
#endif

    prod_test_process();
}
void bsp_led_switch(void);

AT(.text.func.msg.key)
bool func_message_key_event(u16 msg)
{
    bool event_valid = true;

    switch (msg) {
        default:
            /* event invalid */
            event_valid = false;
    }

    if (ble_hid_post_msg_by_key(msg)) {
        return true;
    } else {
        return event_valid;
    }
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    bool event_valid = true;

    switch (msg & MSG_TYPE_MASK) {
        case MSG_TYPE_KEY_EVENT1:
        case MSG_TYPE_KEY_EVENT2:
            #if BSP_KEY_EN
            event_valid = func_message_key_event(msg);
            #endif // BSP_KEY_EN
            break;

//        case MSG_TYPE_SYS_EVENT:
//            /* This type of message may be processed at a previous layer */
//            break;

#if LE_HID_EN
        case MSG_TYPE_HID_EVENT:
            event_valid = func_message_hid_event(msg);
            break;
#endif // LE_HID_EN

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        lowpwr_pwroff_delay_reset();
        lowpwr_sleep_delay_reset();
    }
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    bsp_param_sync();

    lowpwr_sleep_delay_reset();

    lowpwr_pwroff_delay_reset();
}

AT(.text.func)
void func_exit(void)
{
    func_cb.sta = FUNC_IDLE;
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_LE_FCC_EN
        case FUNC_LE_FCC:
            func_le_fcc();
            break;
#endif

#if FUNC_LE_BQB_RF_EN
        case FUNC_LE_BQB_RF:
            func_le_bqb_rf();
            break;
#endif

#if FUNC_IODM_EN
        case FUNC_IODM:
            func_iodm();
            break;
#endif

#if FUNC_TBOX_EN
        case FUNC_TBOX:
            func_tbox();
            break;
#endif

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif

        case FUNC_PWROFF:
            func_pwroff();
            break;

        default:
            func_exit();
            break;
        }
    }
}
