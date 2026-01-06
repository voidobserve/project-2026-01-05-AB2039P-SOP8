#include "include.h"
#include "usb_hid.h"
#include "usb_com.h"


func_cb_t func_cb AT(.buf.func_cb);

AT(.text.app.proc.func)
void func_process(void)
{
    WDT_CLR();


#if USB_EN
    if (sys_cb.usb_is_active) {
        usb_device_process();
    }
#endif // USB_EN

    prod_test_process();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {

#if USB_EN
        case MSG_SYS_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                printf("usb insert\n");
                usb_device_enter();
                sys_cb.usb_is_active = 1;
            }
            break;

        case MSG_SYS_PC_REMOVE:
            printf("usb remove\n");
            usb_device_exit();
            usb_device_deinit();
            break;
#endif // USB_EN

#if BSP_KEY_EN
        case KEY_ID_PP | KEY_LONG_UP:       // PP长按抬起
            printf("PP key long up\n");
            break;

        case KEY_ID_PP | KEY_SHORT_UP:
            printf("PP key short up\n");
        #if USB_HID_REPORT_RATE_TEST
            usb_hid_enable(false);
        #endif
            break;

        case KEY_ID_PP | KEY_DOUBLE:
        #if USB_HID_REPORT_RATE_TEST
            usb_device_resume();
            usb_hid_enable(true);
        #endif
            break;

        case KEY_ID_PP | KEY_THREE:
            printf("PP key three up\n");
        #if USB_HID_EN
            usb_hid_send_test();
        #endif
            break;

#endif // BSP_KEY_EN

        default:
            break;
    }

}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    bsp_param_sync();
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

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif

        default:
            func_exit();
            break;
        }
    }
}
