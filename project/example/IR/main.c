#include "include.h"
#include "ir_example.h"
#include "driver_ir.h"
#include "bsp_uart_debug.h"

#define IR_EXAMPLE 1  //0:IR TX; 1:IR RX

int main(void)
{
    WDT_DIS();

    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

#if (IR_EXAMPLE == 0)
    irtx_example_init();  //只支持软件编码
    printf("IR TX NEC Example\n");
#else
    irrx_example_init();  //硬件解码只支持NEC协议
    printf("IR RX NEC Example\n");
#endif //  IR_EXAMPLE
    while (1) {
#if (IR_EXAMPLE == 0)
    irtx_example();
#else
    irrx_example();
#endif //  IR_EXAMPLE
    }
}
