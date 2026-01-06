#include "include.h"
#include "bsp_uart_debug.h"
#include "usb_hid.h"
#include "usb_com.h"


#define EVT_PC_INSERT                       0x7f7
#define EVT_PC_REMOVE                       0x7f6


u16 tmr5ms_cnt;
u8 is_usb_mode;


AT(.com_text.detect)
void usb_detect(void)
{
    u8 usb_sta = usbchk_host_connect();

    if (usb_sta == USB_HOST_CONNECTED) {
        if (dev_online_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_INSERT);
        }
    } else {
        if (dev_offline_filter(DEV_USBPC)) {
            is_usb_mode = 0;
            msg_enqueue(EVT_PC_REMOVE);
        }
    }
}

AT(.com_text.timer)
void usr_tmr5ms_thread_callback(void)
{
    tmr5ms_cnt++;

    usb_detect();
}

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    sys_clk_set(SYS_120M);

    printf("-------------->usb example\n");

    dev_init(40, 40);
    usb_init();

    usb_device_init();

	sys_set_tmr_enable(1, 0);

    while (1) {
        switch (msg_dequeue()) {
            case EVT_PC_INSERT:
                if (dev_is_online(DEV_USBPC)) {
                    printf("usb insert\n");
                    is_usb_mode = 1;
                    usb_device_enter();
                }
                break;

            case EVT_PC_REMOVE:
                printf("usb remove\n");
                usb_device_exit();
                break;

            default:
                break;
        }

        if (is_usb_mode) {
            usb_device_process();
        }
    }

    return 0;
}
