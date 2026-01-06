#include "include.h"
#include "uart_fot.h"
#include "bsp_param.h"
#include "uart_fot_driver.h"

#if AB_UART_FOT_EN

#define FOT_DEBUG_EN                    1

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif

static fot_s fot_var;
static uint8_t fot_data[FOT_BLOCK_LEN] AT(.fot_data.block_buf);
static uart_fot_rx_packet_t rx_packet AT(.fot_data.rx_buf);
static uart_fot_tx_packet_t tx_packet AT(.fot_data.tx_buf);

static const uint8_t fot_auth_data[] = {0xCC, 0xAA, 0x55, 0xEE, 0x12, 0x19, 0xE4};

static void ab_uart_fot_write(void *buf, uint32_t addr, uint32_t len)
{
    ota_pack_write(buf);
}

static uint8_t ab_uart_fot_get_err(void)
{
    return ota_pack_get_err();
}

static bool ab_uart_fot_breakpoint_info_read(void)
{
    return ota_pack_breakpoint_info_read();
}

static void ab_uart_fot_init(void)
{
    ota_pack_init();
}

static uint32_t ab_uart_fot_get_curaddr(void)
{
    return ota_pack_get_curaddr();
}

static bool ab_uart_fot_is_update_success(void)
{
    return ota_pack_is_update_success();
}

static void uart_fot_init(void)
{
    FOT_DEBUG("%s\n", __func__);
    uint8_t  dev_version_str[] = SW_VERSION;
    uint16_t version_temp = 0;

    memset(&fot_var, 0, sizeof(fot_var));

    fot_var.dev_ver = 0x00;
    version_temp = dev_version_str[1]-'0';
    version_temp <<= 12;
    fot_var.dev_ver |= version_temp;

    version_temp = dev_version_str[3]-'0';
    version_temp <<= 8;
    fot_var.dev_ver |= version_temp;

    version_temp = dev_version_str[5]-'0';
    version_temp <<= 4;
    fot_var.dev_ver |= version_temp;

    fot_var.sta = FOT_STA_INIT;
}

static void uart_fot_exit(void)
{
    if (fot_var.flag & FOT_FLAG_UART_CONNECT) {
        WDT_RST();
    }
}

static void uart_fot_conn_check_restart(void)
{
    fot_var.conn_tmo_tick = tick_get();
}

static uint8_t uart_fot_connect_auth(uint8_t *packet, uint16_t size)
{
    if (!fot_var.sta) {
        if ((size == sizeof(fot_auth_data)) && !memcmp(fot_auth_data, packet, sizeof(fot_auth_data))) {
            uart_fot_init();
            fot_var.flag |= FOT_FLAG_UART_CONNECT;
            uart_fot_conn_check_restart();
            return 0;
        }
    }
    return fot_var.sta;
}

AT(.com_text.uart_fot.packet_receive)
void uart_fot_dma_packet_receive(uint8_t *buf, uint32_t buf_len)
{
    rx_packet.pkt_len = buf_len;
 #if UART_FOT_CRC_EN
    rx_packet.pkt_crc32 = GET_UINT32(&buf[buf_len-UART_PACKET_CRC_LEN]);
#endif
    memcpy(rx_packet.packet, buf, rx_packet.pkt_len);
    fot_var.flag |= FOT_FLAG_DATA_RECEIVE;
}

static void uart_fot_sent_proc(uint8_t *buf,uint8_t len)
{
    FOT_DEBUG("%s\n", __func__);
    if ((fot_var.flag & FOT_FLAG_UART_CONNECT) == 0) {
        return;
    }
#if UART_FOT_CRC_EN
    if (len < (UART_PACKET_MAX_TX_LEN-UART_PACKET_CRC_LEN)) {
        tx_packet.pkt_crc32 = calc_crc32(buf, len, CRC32_SEED);
        PUT_UINT32(&buf[len], tx_packet.pkt_crc32);
        len += UART_PACKET_CRC_LEN;
        hsuart_fot_driver_dma_tx(buf, len);
    } else {
        FOT_DEBUG("packet length error!, len:%d\n", len);
    }
#else
    if (len < UART_PACKET_MAX_TX_LEN) {
        hsuart_fot_driver_dma_tx(buf, len);
    } else {
        FOT_DEBUG("packet length error!, len:%d\n", len);
    }
#endif //UART_FOT_CRC_EN
    FOT_DEBUG_R(buf, len);
}

static void uart_fot_reply_update_request(uint8_t *inbuf, uint8_t *outbuf)
{
    uint16_t flash_remote_ver;
    uint32_t hash;
    uint32_t addr = 0;
    uint32_t block_len = FOT_BLOCK_LEN;
    uint16_t packet_len = FOT_BLOCK_LEN + DATA_START_POS;
    uint8_t write_offset = 0;
    uint8_t need_update = 1;

    ab_uart_fot_init();
    fot_var.sta |= FOT_STA_START;
    fot_var.remote_ver = GET_UINT16(inbuf);
    fot_var.hash = GET_UINT32(&inbuf[2]);
    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_remote_ver_read((uint8_t*)&flash_remote_ver);
    param_fot_hash_read((uint8_t*)&hash);
    FOT_DEBUG("flash hash val:0x%x\n",hash);

    if ((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash)) {
        if (ab_uart_fot_breakpoint_info_read() == true) {
            addr = ab_uart_fot_get_curaddr();
        }
    } else{
        param_fot_addr_write((uint8_t*)&addr);
        param_fot_remote_ver_write((uint8_t*)&fot_var.remote_ver);
        param_fot_hash_write((uint8_t*)&fot_var.hash);
    }

    memcpy(&outbuf[write_offset], &addr, 4);
    write_offset += 4;
    memcpy(&outbuf[write_offset], &block_len, 4);
    write_offset += 4;
    memcpy(&outbuf[write_offset], &packet_len, 2);
    write_offset += 2;
    outbuf[write_offset++] = need_update;
}

static void uart_fot_reply_info_tlv(uint8_t *buf,uint8_t len)
{
    uint8_t read_offset = 0;
    uint8_t write_offset = 0;
    uint8_t val_len = 0;

    if ((buf == NULL) || (len == 0)) {
        return;
    }

    tx_packet.packet[write_offset++] = FOT_GET_INFO_TLV;
    tx_packet.packet[write_offset++] = fot_var.seq++;

    while (read_offset < len) {
        switch (buf[read_offset]) {
            case INFO_DEV_VER:
                FOT_DEBUG("INFO_DEV_VER\n");
                val_len = buf[read_offset + 1];
                tx_packet.packet[write_offset++] = INFO_DEV_VER;
                tx_packet.packet[write_offset++] = 2;
                tx_packet.packet[write_offset++] = fot_var.dev_ver & 0xff;
                tx_packet.packet[write_offset++] = (fot_var.dev_ver >> 8) & 0xff;
            break;

            case INFO_DEV_FEATURE:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                {
                    uint16_t dev_ability = 0;
                    val_len = buf[read_offset + 1];
                    tx_packet.packet[write_offset++] = INFO_DEV_FEATURE;
                    tx_packet.packet[write_offset++] = 2;
                    tx_packet.packet[write_offset++] = dev_ability & 0xff;
                    tx_packet.packet[write_offset++] = (dev_ability >> 8) & 0xff;
                }
            break;

            case INFO_DEV_CONNECT_STA:
                FOT_DEBUG("INFO_DEV_CONNECT_STA\n");
                val_len = buf[read_offset + 1];
                tx_packet.packet[write_offset++] = INFO_DEV_CONNECT_STA;
                tx_packet.packet[write_offset++] = 2;
                tx_packet.packet[write_offset++] = fot_var.con_sta & 0xff;
                tx_packet.packet[write_offset++] = (fot_var.con_sta >> 8) & 0xff;
                break;

            case INFO_UPDATE_REQ:
                FOT_DEBUG("INFO_UPDATE_REQ\n");
                val_len = buf[read_offset + 1];
                tx_packet.packet[write_offset++] = INFO_UPDATE_REQ;
                tx_packet.packet[write_offset++] = 11;
                uart_fot_reply_update_request(&buf[read_offset+2], &tx_packet.packet[write_offset]);
                write_offset += 11;
                break;

            default:
                val_len = buf[read_offset + 1];
                break;
        }
        read_offset += (2 + val_len);
    }

    if (write_offset > sizeof(tx_packet.packet)) {
        printf("fot:uart_fot_tx_packet.packet buf overflow!!!\n");
        while(1);
    }

    tx_packet.pkt_len = write_offset;
    uart_fot_sent_proc(tx_packet.packet, tx_packet.pkt_len);
}

static void uart_fot_dev_notify_sta(uint8_t sta)
{
    uint8_t write_offset = 0;
    tx_packet.packet[write_offset++] = FOT_NOTIFY_STA;
    tx_packet.packet[write_offset++] = fot_var.seq++;
    tx_packet.packet[write_offset++] = sta;

    tx_packet.pkt_len = write_offset;
    uart_fot_sent_proc(tx_packet.packet, tx_packet.pkt_len);
}

uint8_t uart_fot_is_start(void)
{
    return (fot_var.sta & FOT_STA_START) ? 1:0;
}

void uart_fot_pause(void)
{
    if (fot_var.sta & FOT_STA_START) {
        fot_var.sta = FOT_STA_PAUSE;
        uart_fot_dev_notify_sta(FOT_UPDATE_PAUSE);
    }
}

void uart_fot_continue(void)
{
    if (fot_var.sta & FOT_STA_PAUSE) {
        fot_var.sta = FOT_STA_INIT;
        fot_var.total_data_len = 0;
        fot_var.remain_data_len = 0;
        fot_var.data_pos = 0;
        uart_fot_dev_notify_sta(FOT_UPDATE_CONTINUE);
    }
}

//------------------------------------------------------------------------------
static void uart_fot_recv_proc(uint8_t *buf, uint16_t len)
{
    uint32_t addr;
    uint32_t recv_data_len;

    FOT_DEBUG("%s\n", __func__);

    if (fot_var.remote_seq != buf[FOT_SEQ_POS]) {
        printf("remote seq err:%d,%d\n",fot_var.remote_seq, buf[FOT_SEQ_POS]);
        uart_fot_dev_notify_sta(FOT_ERR_SEQ);
        fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
        return;
    }

    fot_var.remote_seq++;

    if ((fot_var.sta & FOT_STA_INIT) == 0) {
        return;
    }

    if (buf[FOT_CMD_POS] == FOT_GET_INFO_TLV) {
        uart_fot_reply_info_tlv(&buf[2], len - 2);
    } else if (buf[FOT_CMD_POS] == FOT_OUT_DATA_START) {
        if (fot_var.remain_data_len) {
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_data_len);
            uart_fot_dev_notify_sta(FOT_ERR_DATA_LEN);
            fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            return;
        }
        recv_data_len = len-DATA_START_POS;
        memcpy(&fot_var.total_data_len, &buf[FOT_DATA_LEN_POS], 4);
        fot_var.remain_data_len = fot_var.total_data_len - recv_data_len;
        memcpy(&addr, &buf[FOT_ADDR_POS], 4);
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_START_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;
        if (fot_var.remain_data_len == 0) {
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    } else if (buf[FOT_CMD_POS] == FOT_OUT_DATA_CONTINUE) {
        recv_data_len = len - DATA_CONTINUE_POS;
        if (fot_var.remain_data_len < recv_data_len) {
           recv_data_len = fot_var.remain_data_len;
        }
        fot_var.remain_data_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_CONTINUE_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;
        if (fot_var.remain_data_len == 0) {
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }
}

static void uart_fot_process_do(void)
{
    if (fot_var.recv_ok) {
        FOT_DEBUG("--->fot_recv_ok\n");
        fot_var.recv_ok = 0;
        ab_uart_fot_write(fot_data, ab_uart_fot_get_curaddr(), fot_var.total_data_len);

        if (ab_uart_fot_is_update_success()) {
            FOT_DEBUG("--->fot update success\n");
            fot_var.flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
            fot_var.update_suc_tick = tick_get();
            uart_fot_dev_notify_sta(FOT_UPDATE_DONE);
        } else{
            uart_fot_dev_notify_sta(ab_uart_fot_get_err());
            if (ab_uart_fot_get_err()) {
                fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            }
        }
    }
}

bool uart_fot_check_rx_crc(uint8_t *packet, uint16_t len)
{
    uint32_t temp_crc = calc_crc32(packet, len-UART_PACKET_CRC_LEN, CRC32_SEED);
    bool res = (temp_crc == rx_packet.pkt_crc32) ? 1 : 0;
    return res;
}

static void uart_fot_flag_do(void)
{
    if (fot_var.flag & FOT_FLAG_DATA_RECEIVE) {
        FOT_DEBUG("FOT_FLAG_DATA_RECEIVE, len:%d\n", rx_packet.pkt_len);
        FOT_DEBUG_R(rx_packet.packet, rx_packet.pkt_len);
#if UART_FOT_CRC_EN
        if (uart_fot_check_rx_crc(rx_packet.packet, rx_packet.pkt_len) == 1) {
            if (uart_fot_connect_auth(rx_packet.packet, rx_packet.pkt_len - UART_PACKET_CRC_LEN)) {
                uart_fot_recv_proc(rx_packet.packet, rx_packet.pkt_len - UART_PACKET_CRC_LEN);
                uart_fot_conn_check_restart();
            }
        } else {
            uart_fot_dev_notify_sta(FOT_ERR_CRC_VERIFY);
            FOT_DEBUG("%s, crc err!!\n", __func__);
        }
#else
        if (uart_fot_connect_auth(rx_packet.packet, rx_packet.pkt_len)) {
            uart_fot_recv_proc(rx_packet.packet, rx_packet.pkt_len);
            uart_fot_conn_check_restart();
        }
#endif //UART_FOT_CRC_EN
        fot_var.flag &= ~FOT_FLAG_DATA_RECEIVE;
    }

    if ((fot_var.flag & FOT_FLAG_UART_CONNECT) && !(fot_var.flag & FOT_FLAG_UPDATE_OK)) {
        if (tick_check_expire(fot_var.conn_tmo_tick, CONN_TIMEOUT)) {
            fot_var.flag &= ~FOT_FLAG_UART_CONNECT;
            FOT_DEBUG("-->fota conn timeout,sys reset\n");
            WDT_RST();
        }
    }

    if (fot_var.flag & FOT_FLAG_SYS_RESET) {
        if (tick_check_expire(fot_var.update_suc_tick, 3000)) {
            fot_var.flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("-->fota update ok,sys reset\n");
            WDT_RST();
        }
    }

    if (fot_var.flag & FOT_FLAG_UPDATE_EXIT) {
        uart_fot_exit();
        fot_var.flag &= ~FOT_FLAG_UPDATE_EXIT;
    }
}

void uart_fot_process(void)
{
    if (fot_var.sta & FOT_STA_START) {
        // lowpwr_pwroff_delay_reset();  //标准SDK需要打开
        // lowpwr_sleep_delay_reset();   //标准SDK需要打开
        uart_fot_process_do();
    }

    if (fot_var.flag) {
        uart_fot_flag_do();
    }
}

#endif //AB_UART_FOT_EN
