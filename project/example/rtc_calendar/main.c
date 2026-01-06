#include "include.h"
#include "rtc_calendar.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->rtc calendar example\n");
    
    rtc_calendar_init();

    while (1) {
        rtc_calendar_example();
    }

    return 0;
}
