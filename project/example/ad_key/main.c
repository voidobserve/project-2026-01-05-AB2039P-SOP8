#include "include.h"
#include "ad_key.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->ad_key example\n");
    
    ad_key_init();

    while (1) {
        ad_key_example();
    }

    return 0;
}
