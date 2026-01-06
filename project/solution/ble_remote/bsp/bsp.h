#ifndef _BSP_H
#define _BSP_H

#include "bsp_param.h"
#include "bsp_sys.h"
#include "ble_init.h"
#include "ble_proc.h"
#include "bsp_uart_debug.h"
#include "bsp_saradc.h"
#include "bsp_saradc_vbat.h"
#include "bsp_saradc_tsen.h"
#include "bsp_key.h"
#include "bsp_huart_iodm.h"
#include "func.h"
#include "msg.h"

#include "prod_test.h"

#if BSP_IR_TX_EN
#include "bsp_ir_tx.h"
#endif  //BSP_IR_TX_EN
#if BSP_IR_RX_EN
#include "bsp_ir_rx.h"
#endif  //BSP_IR_RX_EN

#endif
