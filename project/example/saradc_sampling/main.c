#include "include.h"
#include "saradc_sampling.h"
#include "bsp_uart_debug.h"

/*
  BIT[7:1]: Reserve.
  BIT[0]:   Buck enable bit. it's buck mode when it's 1, or it will be ldo mode.
 */
#define PMU_TYPE        0x00    // 0: LDO
                                // 1: BUCK

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);

    printf("--->saradc sampling example\n");

    pmu_init(PMU_TYPE);

    sys_clk_set(SYS_48M);

    saradc_sampling_init();

    while (1) {
        saradc_sampling_run();
    }

    return 0;
}
