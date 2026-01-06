#include "include.h"
#include "bsp_uart_debug.h"
#include "uart_fot_driver.h"
#include "uart_fot.h"
#include "bsp_param.h"

int main(void)
{
    WDT_DIS();
    
    u32 rst_reason = sys_rst_init(0);

    bsp_uart_debug_init();

    sys_rst_dump(rst_reason);
   
    sys_clk_set(SYS_60M);   // When selecting clock PLLDIV4, ensure that the sys_clk is SYS_60M or higher.

    bsp_param_init();
	
#if AB_UART_FOT_EN       
    hsuart_fot_driver_init();
#endif //AB_UART_FOT_EN 
  
    printf("--->uart fot example\n");

    while (1) {
        WDT_DIS();
        delay_5ms(1);
		
#if AB_UART_FOT_EN     
        uart_fot_process();
#endif //AB_UART_FOT_EN        
    }

    return 0;
}
