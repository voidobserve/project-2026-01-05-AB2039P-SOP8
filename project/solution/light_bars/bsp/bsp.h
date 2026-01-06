#ifndef _BSP_H
#define _BSP_H

#include "bsp_param.h"
#include "bsp_sys.h"
#include "ble_init.h"
#include "ble_proc.h"
#include "bsp_uart_debug.h"
#include "bsp_huart_iodm.h"
#include "func.h"
#include "msg.h"

#include "ble_app.h"

#if BSP_IR_EN
#include "bsp_ir.h"
#endif

#if BSP_LEDC_EN
#include "bsp_ws2815_ledc.h"
#endif

#if BSP_MIC_EN
#include "bsp_saradc.h"
#endif

#include "prod_test.h"

#endif
