#include "include.h"

#define TRACE_EN                    0

#if TRACE_EN
#define TRACE_KEY(...)              printf("[KEY] "); \
                                    printf(__VA_ARGS__)
#else
#define TRACE_KEY(...)
#endif


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

#if BSP_GSENSOR_DETECT_EN
    bsp_gsensor_process();
#endif

#if BSP_PWM_EN
    bsp_pwm_process();
#endif

    prod_test_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    bool event_valid = true;

    switch (msg) {

#if BSP_KEY_EN
        case KEY_ID_PP | KEY_SHORT_UP:      // PP按键短按抬起
            TRACE_KEY("PP key short key up\n");
            key_event |= MSG_SEND_GO_HOME_CMD;
            // key_event |= MSG_SEND_VOL_INC_CMD;
            // key_event |= MSG_SEND_PEN_SWITCH_CMD;
            break;

        case KEY_ID_PP | KEY_LONG:          // PP长按
            TRACE_KEY("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_HOLD:          // PP连按
            TRACE_KEY("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_LONG_UP:       // PP长按抬起
            TRACE_KEY("PP key long up\n");
            func_cb.sta = FUNC_PWROFF;
            break;

        case KEY_ID_PP | KEY_DOUBLE:        // PP双击
            TRACE_KEY("PP key double down\n");
            key_event |= MSG_SEND_MUTITASK_BAR_CMD;
            // key_event |= MSG_SEND_ERASER_SWITCH_CMD;
            // key_event |= MSG_SEND_LOCK_WAKE_SCREEN_CMD;
            break;

        case KEY_ID_PP | KEY_THREE:         // PP三击
            TRACE_KEY("PP key triple down\n");
            key_event |= MSG_SEND_SEARCH_BAR;
            // key_event |= MSG_SEND_VOL_DEC_CMD;
            // key_event |= MSG_SEND_UNDO_CMD;
            break;

        case KEY_ID_K1 | KEY_SHORT_UP:         // K1短按抬起
            TRACE_KEY("K1 key short key up\n");
            break;

        case KEY_ID_K2 | KEY_SHORT_UP:         // K2短按抬起
            TRACE_KEY("K2 key short key up\n");
            break;

        case KEY_ID_K3 | KEY_SHORT_UP:         // K3短按抬起
            TRACE_KEY("K3 key short key up\n");
            break;

        case KEY_ID_K4 | KEY_SHORT_UP:         // K4短按抬起
            TRACE_KEY("K4 key short key up\n");
            break;
#endif

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
