#include "include.h"
#include "usb_com.h"
#include "fota_proc.h"


func_cb_t func_cb AT(.buf.func_cb);

AT(.text.app.proc.func)
void func_process(void)
{
    WDT_CLR();

#if BSP_VBAT_DETECT_EN
    bsp_vbat_process();
#endif

#if USB_EN
    usb_device_process();
#endif

#if AB_FOT_EN
    fota_process();
#endif

    prod_test_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    bool event_valid = true;

    switch (msg) {

#if USB_EN
        case MSG_SYS_PC_INSERT: {
            if (dev_is_online(DEV_USBPC)) {
                printf("usb insert\n");
                usb_device_enter();
            }
        } break;

        case MSG_SYS_PC_REMOVE: {
            printf("usb remove\n");
            usb_device_exit();
            usb_device_deinit();
        } break;
#endif

#if BSP_KEY_EN
        case KEY_ID_PP | KEY_SHORT_UP:      // PP按键短按抬起
            printf("PP key short key up\n");
            break;

        case KEY_ID_PP | KEY_LONG:          // PP长按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_HOLD:          // PP连按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_LONG_UP:       // PP长按抬起
            printf("PP key long up\n");
            func_cb.sta = FUNC_PWROFF;
            break;

        case KEY_ID_PP | KEY_DOUBLE:        // PP双击
            printf("PP key double down\n");
            break;

        case KEY_ID_PP | KEY_THREE:         // PP三击
            printf("PP key triple down\n");
            break;

        case KEY_ID_K1 | KEY_SHORT_UP:         // K1短按抬起
            printf("K1 key short key up\n");
            break;

        case KEY_ID_K2 | KEY_SHORT_UP:         // K2短按抬起
            printf("K2 key short key up\n");
            break;

        case KEY_ID_K3 | KEY_SHORT_UP:         // K3短按抬起
            printf("K3 key short key up\n");
            break;

        case KEY_ID_K4 | KEY_SHORT_UP:         // K4短按抬起
            printf("K4 key short key up\n");
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

#if FUNC_WIRELESS_EN
        case FUNC_WIRELESS:
            func_wireless();
            break;
#endif


#if FUNC_IODM_EN
        case FUNC_IODM:
            func_iodm();
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
