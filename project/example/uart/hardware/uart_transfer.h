#ifndef _UART_TRANSFER_H
#define _UART_TRANSFER_H


typedef struct {
    u8 done;
    u16 len;
    u8 *buf;
} uart_transfer_cb_typedef;


void uart_transfer_init(u32 baud);
void uart_transfer_example(void);

#endif // _UART_TRANSFER_H
