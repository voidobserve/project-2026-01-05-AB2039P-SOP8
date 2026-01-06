#include "include.h"
#include "driver_hsuart.h"
#include "driver_gpio.h"
#include "driver_com.h"
#include "uart_fot_driver.h"
#include "uart_fot.h"

#if AB_UART_FOT_EN

static uint8_t hsuart_fot_driver_rx_buffer[HSUART_FOT_DRIVER_RX_BUF_LEN]; 
static uint8_t hsuart_fot_driver_tx_buffer[HSUART_FOT_DRIVER_TX_BUF_LEN]; 

///HSUART 等发送空闲
AT(.com_text.hsuart_fot_driver.text)
static void hsuart_fot_driver_wait_txdone(void)
{
    while (hsuart_get_flag(HSUART_IT_TX) == RESET);
}

///HSUART 发送数据
void hsuart_fot_driver_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len)
{
    hsuart_fot_driver_wait_txdone();
    memcpy(hsuart_fot_driver_tx_buffer, dma_buf, dma_buf_len);
    hsuart_dma_start(HSUT_TRANSMIT, DMA_ADR(hsuart_fot_driver_tx_buffer), dma_buf_len);
}

///HSUART DMA 中断服务函数
AT(.com_text.hsuart_fot_driver.dma_rx_isr)
static void hsuart_fot_driver_dma_rx_isr(void)
{
    if (hsuart_get_flag(HSUART_IT_RX) != RESET) {
        uart_fot_dma_packet_receive(hsuart_fot_driver_rx_buffer, hsuart_get_fifo_counter());
        hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
        hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(hsuart_fot_driver_rx_buffer), HSUART_FOT_DRIVER_RX_BUF_LEN);
    }
}

void hsuart_fot_driver_init(void)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

/************************************ GPIO Init ************************************/
    gpio_init_structure.gpio_pin = HSUART_FOT_DRIVER_PIN_SEL;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_FOT_DRIVER_PORT_SEL, &gpio_init_structure);

    /* NOTE: make sure printf_io isn't using PB9(VUSB) */
    gpio_func_mapping_clear(HSUART_FOT_DRIVER_PORT_SEL, HSUART_FOT_DRIVER_PIN_SEL);
    gpio_func_mapping_config(HSUART_FOT_DRIVER_PORT_SEL, HSUART_FOT_DRIVER_PIN_SEL, GPIO_CROSSBAR_IN_HURRX);
    gpio_func_mapping_config(HSUART_FOT_DRIVER_PORT_SEL, HSUART_FOT_DRIVER_PIN_SEL, GPIO_CROSSBAR_OUT_HURTX);

/*********************************** HSUART Init ***********************************/
    hsuart_init_struct.baud = HSUART_FOT_DRIVER_BAUD;
    hsuart_init_struct.one_line_en = HSUT_ONELINE_EN;
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_2BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_24M_CLK;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_SINGLE;
    hsuart_init(&hsuart_init_struct);
    hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
    hsuart_rx_idle_config(HSUART_FOT_DRIVER_OV_CNT, ENABLE);
    hsuart_pic_config(hsuart_fot_driver_dma_rx_isr, 0, HSUART_IT_RX, ENABLE);
    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);
    hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(hsuart_fot_driver_rx_buffer), HSUART_FOT_DRIVER_RX_BUF_LEN);
}

void hsuart_fot_driver_exit(void)
{
    
}

#endif //AB_UART_FOT_EN