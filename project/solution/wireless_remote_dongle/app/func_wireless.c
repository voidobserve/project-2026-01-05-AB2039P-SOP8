#include "include.h"

#if FUNC_WIRELESS_EN

AT(.text.func.msg)
static void func_wireless_message(uint16_t msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            break;

        case MSG_SYS_500MS:
            break;

        default:
            func_message(msg);
            break;
    }
}

AT(.text.app.proc.wireless)
static void func_wireless_process(void)
{
    func_process();

    wireless_client_process();

#if SYS_PWROFF_TIME
    if (sys_cb.pwroff_delay == 0) {
        func_cb.sta = FUNC_PWROFF;
        return;
    }
#endif

#if SYS_SLEEP_EN
    if (sleep_process(ble_is_sleep)) {

    }
#endif
}

AT(.text.func.wireless)
static void func_wireless_enter(void)
{
    msg_queue_clear();

    wireless_module_init();
    wireless_client_init();
}

AT(.text.func.wireless)
static void func_wireless_exit(void)
{
    wireless_module_deinit();

    func_cb.last = FUNC_WIRELESS;
}

AT(.text.func.wireless)
void func_wireless(void)
{
    printf("%s\n", __func__);

    func_wireless_enter();

    while (func_cb.sta == FUNC_WIRELESS) {
        func_wireless_process();
        func_wireless_message(msg_dequeue());
    }

    func_wireless_exit();
}

#endif // FUNC_WIRELESS_EN
