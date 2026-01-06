#ifndef __BLE_FOTA_SERVICE_H
#define __BLE_FOTA_SERVICE_H
#include "config.h"

#if AB_UART_FOT_EN

#define UART_FOT_CRC_EN         1               //是否使能CRC校验

#define FOT_STA_INIT            BIT(0)
#define FOT_STA_START           BIT(1)
#define FOT_STA_PAUSE           BIT(2)

#define FOT_FLAG_UPDATE_OK      BIT(0)
#define FOT_FLAG_SYS_RESET      BIT(1)
#define FOT_FLAG_UART_CONNECT   BIT(2) 
#define FOT_FLAG_CLK_SET        BIT(3)
#define FOT_FLAG_UPDATE_EXIT    BIT(4)
#define FOT_FLAG_DATA_RECEIVE   BIT(5)

#define FOT_NOTIFY_STA          0x90
#define FOT_GET_INFO            0x91
#define FOT_GET_INFO_TLV        0x92
#define FOT_OUT_DATA_START      0xA0
#define FOT_OUT_DATA_CONTINUE   0x20

#define FOT_CMD_POS             0
#define FOT_SEQ_POS             1
#define FOT_ADDR_POS            2
#define FOT_DATA_LEN_POS        6
#define DATA_START_POS          10
#define DATA_CONTINUE_POS       2

#define FOT_HEAD_LEN            10
#define FOT_BLOCK_LEN           512
#define UART_PACKET_CRC_LEN     4
#if UART_FOT_CRC_EN
#define UART_PACKET_MAX_RX_LEN  (FOT_HEAD_LEN+FOT_BLOCK_LEN+UART_PACKET_CRC_LEN)
#else
#define UART_PACKET_MAX_RX_LEN  (FOT_HEAD_LEN+FOT_BLOCK_LEN)
#endif //UART_FOT_CRC_EN
#define UART_PACKET_MAX_TX_LEN  50

#define SW_VERSION		        "V1.0.1"		//只能使用数字0-9,ota需要转码
#define CONN_TIMEOUT            10000           //ms

#define CRC32_SEED              0xffffffff

#define GET_UINT16(ptr)         (u16)(*(u16*)(u8*)(ptr))
#define PUT_UINT16(ptr, val)    *(u16*)(u8*)(ptr) = (u16)(val)
#define GET_UINT32(ptr)         (uint32_t)(*(uint32_t*)(uint8_t*)(ptr))
#define PUT_UINT32(ptr, val)    *(uint32_t*)(uint8_t*)(ptr) = (uint32_t)(val)

typedef enum{
    INFO_DEV_VER  = 1,
    INFO_UPDATE_REQ,
    INFO_DEV_FEATURE,
    INFO_DEV_CONNECT_STA,
    INFO_EDR_ADDR,
    INFO_DEV_CHANNEL,
}DEV_INFO_E;

typedef struct {
    uint8_t packet[UART_PACKET_MAX_RX_LEN];
    uint16_t pkt_len;
    uint32_t pkt_crc32;
} uart_fot_rx_packet_t;

typedef struct {
    uint8_t packet[UART_PACKET_MAX_TX_LEN];
    uint16_t pkt_len;
    uint32_t pkt_crc32;
} uart_fot_tx_packet_t;

typedef struct __attribute__((packed)){
    uint8_t seq;
    uint8_t remote_seq;
    uint8_t sta;
    uint8_t flag;
    uint16_t con_sta;
    uint16_t dev_ver;
    uint16_t remote_ver;
    uint32_t recv_pkt_len;
    uint32_t total_data_len;
    uint32_t remain_data_len;
    uint32_t hash;
    uint16_t data_pos;
    uint8_t recv_ok;
    uint32_t addr;
    uint32_t update_suc_tick;
    uint32_t conn_tmo_tick;
}fot_s;

uint32_t calc_crc32(uint8_t *buf, uint len, uint32_t crc_key);
void uart_fot_dma_packet_receive(uint8_t *buf, uint32_t buf_len);
void uart_fot_process(void);
#endif

#endif //AB_UART_FOT_EN