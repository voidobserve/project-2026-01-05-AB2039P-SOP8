/**
******************************************************************************************************************************
*
*@file irrx_example.c
*
*@brief Source file for IR rx example
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/

#include "include.h"
#include "driver_ir.h"
#include "driver_gpio.h"

#if (BSP_IR_EN)

static uint8_t irrx_done = 0;

AT(.com_text.irq)
void irrx_done_set(void)
{
    irrx_done = 1;
}

void irrx_done_clr(void)
{
    irrx_done = 0;
}

uint8_t irrx_done_get(void)
{
    return irrx_done;
}

AT(.com_text.irq)
void bsp_irrx_irq(void)     //解码中断
{
    // if (irrx_get_flag(IRRX_FLAG_RXPND)) {
    //     irrx_clear_flag(IRRX_FLAG_RXPND);
    //     irrx_flag = 1;
    //     irrx_data = irrx_get_data();
    // }

    // if (irrx_get_flag(IRRX_FLAG_KEYRELS)) {
    //     irrx_clear_flag(IRRX_FLAG_KEYRELS);
    //     irrx_flag = 2;
    //     irrx_data = 0;
    // }
}

/*
void bsp_ir_clk_out_init(void)
{
    gpio_init_typedef gpio_init_structure = {0};

    gpio_init_structure.gpio_pin = IRCLK_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    //gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IRCLK_IO_PORT, &gpio_init_structure);

    gpio_func_mapping_config(IRCLK_IO_PORT, IRCLK_IO_PIN, GPIO_CROSSBAR_OUT_CLKOUT);
}
*/

void bsp_irrx_decode_init(void)
{
    gpio_init_typedef gpio_init_structure;
    irrx_param_t irrx_param;

    clk_gate1_cmd(CLK_GATE1_IRRX, CLK_EN);
    clk_irrx_clk_set(CLK_IR_CLK_X24M_CLKDIV24);

    gpio_init_structure.gpio_pin = IRRX_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;

    gpio_init(IRRX_IO_PORT, &gpio_init_structure);

    gpio_func_mapping_config(IRRX_IO_PORT, IRRX_IO_PIN, GPIO_CROSSBAR_IN_IRRX);

    irrx_param.wakup_en = IRRX_WAKUP_SLEEP_ENABLE;
    irrx_param.data_format = IRRX_DATA_SELECT_32BIT;

    irrx_param.filter_en = IRRX_FILTER_ENABLE;
    irrx_param.filter_srcs = IRRX_FILTER_SRCS_RXIN;
    irrx_param.filter_len = 3;

    irrx_init(&irrx_param);

    // irrx_pic_config(bsp_irrx_irq, 0, IRRX_FLAG_RXPND, ENABLE);

    irrx_cmd(ENABLE);

    // ir clock io init
    //bsp_ir_clk_out_init();
}

/**
  * @brief  bsp_irrx_handler_process.Call it in func_process
  * @retval None
  */
void bsp_irrx_handler_process(void)
{
    static uint32_t irrx_query_tick = 0;
    static uint16_t ir_press_cnt = 0;
    
    if (tick_check_expire(irrx_query_tick, 10)) {
        irrx_query_tick = tick_get();
        if(irrx_get_flag(IRRX_FLAG_RXPND)) {
            ir_press_cnt ++;
            //在release pnd到来之前检测是否是长按
            if(irrx_get_flag(IRRX_FLAG_KEYRELS)) {
                uint32_t irrx_dat = irrx_get_data();
                printf ("%s, irrx_dat = %08X, ir_press_cnt %d\n", __func__, irrx_dat, ir_press_cnt);
            
                if (ir_press_cnt > 200) {
                    irrx_dat |= 0xFF;       //最低位或上0xff，以区分长按短按
                    printf ("%s, key long press, irrx_dat = %08X\n", __func__, irrx_dat);
                }
                
                if (irrx_dat == 0x50000908) {
                    bsp_ws2815_app_set(1);
                } else if (irrx_dat == 0x50000900) {
                    bsp_ws2815_app_set(0);
                }
                
                ir_press_cnt = 0;
                irrx_clear_flag(IRRX_FLAG_RXPND | IRRX_FLAG_KEYRELS);
            }
        } else {
            ir_press_cnt = 0;
        }
    }
}

#endif //#if (BSP_IR_EN)
