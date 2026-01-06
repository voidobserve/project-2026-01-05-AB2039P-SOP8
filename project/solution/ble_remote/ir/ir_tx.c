#include "include.h"
#include "ir_tx.h"

#if BSP_IR_TX_EN

#define IRTX_IO_PORT             GPIO_PORT_GET(BSP_IR_TX_EN)
#define IRTX_IO_PIN              GPIO_PIN_GET(BSP_IR_TX_EN)

void ir_tx_init(void)
{
    bsp_irtx_init(IRTX_IO_PORT, IRTX_IO_PIN);
}

static void ir_tx_cmd_proc(KEY_MSG_TYPEDEF key_msg)
{
    uint32_t ir_key_cmd = IR_KEY_NONE_CMD;

    switch (key_msg) {
        case KEY_MSG_UP:
            ir_key_cmd = IR_KEY_UP_CMD;
            break;
        case KEY_MSG_DOWN:
            ir_key_cmd = IR_KEY_DOWN_CMD;
            break;
        case KEY_MSG_LEFT:
            ir_key_cmd = IR_KEY_LEFT_CMD;
            break;
        case KEY_MSG_RIGHT:
            ir_key_cmd = IR_KEY_RIGHT_CMD;
            break;
        case KEY_MSG_MEUN:
            ir_key_cmd = IR_KEY_MEUN_CMD;
            break;
        case KEY_MSG_OK:
            ir_key_cmd = IR_KEY_OK_CMD;
            break;
        case KEY_MSG_BACK:
            ir_key_cmd = IR_KEY_BACK_CMD;
            break;
        case KEY_MSG_VOL_UP:
            ir_key_cmd = IR_KEY_VOL_UP_CMD;
            break;
        case KEY_MSG_VOL_DOWN:
            ir_key_cmd = IR_KEY_VOL_DOWN_CMD;
            break;
        case KEY_MSG_POWER:
            ir_key_cmd = IR_KEY_PWR_CMD;
            break;

        default:
            break;
    }

    if (ir_key_cmd != IR_KEY_NONE_CMD) {
        bsp_key_set_msg(KEY_MSG_NONE);
        bsp_itx_send_data(0x4E, ir_key_cmd, 0);
    }
}

void ir_tx_proc(void)
{
    if (ble_cb.con_handle == 1) {
        return;
    }

    KEY_MSG_TYPEDEF key_msg = bsp_key_get_msg();

    if (key_msg != KEY_MSG_NONE){
        lowpwr_sleep_delay_reset();
        ir_tx_cmd_proc(key_msg);
    }
}

#endif  //BSP_IR_TX_EN