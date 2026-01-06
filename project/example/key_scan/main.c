#include "include.h"
#include "key_scan.h"
#include "bsp_uart_debug.h"


int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->key scan example\n");
    
    key_scan_init();

    while (1) {
        key_scan_example();
    }

    return 0;
}
