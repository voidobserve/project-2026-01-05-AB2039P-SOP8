#ifndef _UART_FOT_DRIVER_H
#define _UART_FOT_DRIVER_H
#include "uart_fot.h"

#if AB_UART_FOT_EN

#define HSUART_FOT_DRIVER_RX_BUF_LEN          UART_PACKET_MAX_RX_LEN
#define HSUART_FOT_DRIVER_TX_BUF_LEN          UART_PACKET_MAX_TX_LEN

#define HSUART_FOT_DRIVER_BAUD                1000000
#define HSUART_FOT_DRIVER_PORT_SEL            GPIOB_REG
#define HSUART_FOT_DRIVER_PIN_SEL             GPIO_PIN_9  //VUSB
#define HSUART_FOT_DRIVER_OV_CNT              0x20        //unit: time for transmitting 1 bit

void hsuart_fot_driver_init(void);
void hsuart_fot_driver_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len);

#endif  //AB_UART_FOT_EN

#endif  //_UART_FOT_DRIVER_H
