#include "include.h"
#include "spi1_example.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();

    sys_clk_set(SYS_24M);
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    while(1) {
        spi1_example();
    }

    return 0;
}
