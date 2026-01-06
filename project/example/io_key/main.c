#include "include.h"
#include "io_key.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    // power init
    pmu_init(0);
    
    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->io key example\n");
    
    io_key_init();

    while (1) {
        io_key_example();
    }

    return 0;
}
