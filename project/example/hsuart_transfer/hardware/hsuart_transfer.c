#include "include.h"
#include "driver_com.h"
#include "driver_hsuart.h"
#include "driver_gpio.h"
#include "hsuart_transfer.h"


#define HSUART_ONELINE_EN           0    //0:双线串口； 1：单线串口
//单线
#define HSUART_TXRX_PORT            GPIOB_REG
#define HSUART_TXRX_PIN             GPIO_PIN_5
//双线
#define HSUART_TX_PORT              GPIOB_REG
#define HSUART_TX_PIN               GPIO_PIN_5
#define HSUART_RX_PORT              GPIOB_REG
#define HSUART_RX_PIN               GPIO_PIN_6

#define HSUART_HW_FLOW_CTRL_EN      0    //流控使能
#define HSUART_HW_FC_RTS_PORT       GPIOB_REG
#define HSUART_HW_FC_RTS_PIN        GPIO_PIN_7
#define HSUART_HW_FC_CTS_PORT       GPIOB_REG
#define HSUART_HW_FC_CTS_PIN        GPIO_PIN_8

#define RECV_DMA_BUF_MAX_LEN        100
static volatile bool hsuart_rx_flag = 0;
static uint8_t hsuart_dma_rx_buf[RECV_DMA_BUF_MAX_LEN];
static uint8_t hsuart_dma_tx_buf[RECV_DMA_BUF_MAX_LEN];
static uint8_t hsuart_user_buf[RECV_DMA_BUF_MAX_LEN];
static uint8_t hsuart_user_buf_len = 0;

void hsuart_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len)
{
    memcpy(hsuart_dma_tx_buf, dma_buf, dma_buf_len);
    hsuart_dma_start(HSUT_TRANSMIT, DMA_ADR(hsuart_dma_tx_buf), dma_buf_len);
    while (hsuart_get_flag(HSUART_IT_TX) == RESET);
}

AT(.com_periph.hsuart.isr)
void hsuart_dma_rx_isr()
{
	if(hsuart_get_flag(HSUART_IT_RX)){
        hsuart_rx_flag = 1;
		hsuart_user_buf_len = hsuart_get_fifo_counter();
        memcpy(hsuart_user_buf, hsuart_dma_rx_buf, hsuart_user_buf_len);
		hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
        hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(hsuart_dma_rx_buf), RECV_DMA_BUF_MAX_LEN);
	}
}

void hsuart_transfer_init(u32 baud)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

#if HSUART_ONELINE_EN
    //TX RX
    gpio_init_structure.gpio_pin = HSUART_TXRX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_TXRX_PORT, &gpio_init_structure);

    gpio_func_mapping_clear(HSUART_TXRX_PORT, HSUART_TXRX_PIN);
    gpio_func_mapping_config(HSUART_TXRX_PORT, HSUART_TXRX_PIN, GPIO_CROSSBAR_OUT_HURTX);
    gpio_func_mapping_config(HSUART_TXRX_PORT, HSUART_TXRX_PIN, GPIO_CROSSBAR_IN_HURRX);
#else
    // Rx
    gpio_init_structure.gpio_pin = HSUART_RX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_RX_PORT, &gpio_init_structure);

    // Tx
    gpio_init_structure.gpio_pin = HSUART_TX_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(HSUART_TX_PORT, &gpio_init_structure);

    gpio_func_mapping_clear(HSUART_TX_PORT, HSUART_TX_PIN);
    gpio_func_mapping_clear(HSUART_RX_PORT, HSUART_RX_PIN);
    gpio_func_mapping_config(HSUART_TX_PORT, HSUART_TX_PIN, GPIO_CROSSBAR_OUT_HURTX);
    gpio_func_mapping_config(HSUART_RX_PORT, HSUART_RX_PIN, GPIO_CROSSBAR_IN_HURRX);
#endif  //HSUART_ONELINE_EN

    hsuart_init_struct.baud = baud;
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_SINGLE;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_1BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_24M_CLK;   // When selecting clock PLLDIV4, ensure that the sys_clk is SYS_60M or higher.

#if HSUART_ONELINE_EN
    hsuart_init_struct.one_line_en = HSUT_ONELINE_EN;
#else
    hsuart_init_struct.one_line_en = HSUT_ONELINE_DIS;
#endif  //HSUART_ONELINE_EN

#if HSUART_HW_FLOW_CTRL_EN
    gpio_init_structure.gpio_pin = HSUART_HW_FC_RTS_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_HW_FC_RTS_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = HSUART_HW_FC_CTS_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_HW_FC_RTS_PORT, &gpio_init_structure);

    gpio_func_mapping_clear(HSUART_HW_FC_RTS_PORT, HSUART_HW_FC_RTS_PIN);
    gpio_func_mapping_clear(HSUART_HW_FC_CTS_PORT, HSUART_HW_FC_CTS_PIN);
    gpio_func_mapping_config(HSUART_HW_FC_RTS_PORT, HSUART_HW_FC_RTS_PIN, GPIO_CROSSBAR_OUT_HURRTS);
    gpio_func_mapping_config(HSUART_HW_FC_CTS_PORT, HSUART_HW_FC_CTS_PIN, GPIO_CROSSBAR_IN_HURCTS);
    hsuart_init_struct.hardware_flow_ctrl_mode = HSUT_HFC_CTS_RTS;
#else
    hsuart_init_struct.hardware_flow_ctrl_mode = HSUT_HFC_NONE;
#endif  //HSUART_HW_FLOW_CTRL_EN

    hsuart_init(&hsuart_init_struct);

    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);

    hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(hsuart_dma_rx_buf), RECV_DMA_BUF_MAX_LEN);

	hsuart_pic_config(hsuart_dma_rx_isr, 0, HSUART_IT_RX | HSUART_IT_RX_TMR_OV, ENABLE);

	hsuart_rx_idle_config(50, ENABLE);  // 空闲中断
}


void hsuart_transfer_example(void)
{
    static uint32_t tick = -1;
   if (tick_check_expire(tick, 10)) {
        tick = tick_get();
        if (hsuart_rx_flag) {
            hsuart_rx_flag = 0;
            //接收输出
            printf("receive data[%d]: ", hsuart_user_buf_len);
            print_r(hsuart_user_buf, hsuart_user_buf_len);

            //回传测试
            hsuart_dma_tx(hsuart_user_buf, hsuart_user_buf_len);
        }
   }
}
