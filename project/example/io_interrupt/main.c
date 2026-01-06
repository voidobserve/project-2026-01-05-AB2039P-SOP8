#include "include.h"
#include "io_interrupt.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->io interrupt example\n");

    io_interrupt_init();

    return 0;
}
