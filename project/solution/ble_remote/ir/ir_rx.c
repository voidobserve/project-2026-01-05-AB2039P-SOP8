#include "include.h"
#include "ir_rx.h"

#if BSP_IR_RX_EN

#define IRRX_IO_PORT             GPIO_PORT_GET(BSP_IR_RX_EN)
#define IRRX_IO_PIN              GPIO_PIN_GET(BSP_IR_RX_EN)

void ir_rx_init(void)
{
    //硬件解码只支持 NEC 协议
    bsp_irrx_init(IRRX_IO_PORT, IRRX_IO_PIN);
}

// static void ir_rx_cmd_proc(uint8_t ir_cmd)
// {
//     switch (ir_cmd) {

//         default:
//             break;
//     }
// }

void ir_rx_proc(void)
{
    if (ble_cb.con_handle == 1) {
        return;
    }

    bsp_irrx_proc();
    // ir_rx_cmd_proc();
}

#endif  //BSP_IR_RX_EN