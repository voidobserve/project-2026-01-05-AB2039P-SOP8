#include "include.h"
#include "hsuart_transfer.h"
#include "bsp_uart_debug.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->hsuart transfer example\n");
    
    hsuart_transfer_init(115200);

    while (1) {
        hsuart_transfer_example();
    }

    return 0;
}
