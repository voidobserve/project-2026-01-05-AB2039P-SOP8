#include "include.h"
#include "ledc.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->ledc ledc example\n");

    ledc_example_init();
    ledc_example_loop();

    return 0;
}
