#include "include.h"
#include "pwm.h"
#include "bsp_uart_debug.h"



int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    // bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->pwm example\n");

    pwm_init();

    while (1) {
        pwm_example();
    }
    return 0;
}
