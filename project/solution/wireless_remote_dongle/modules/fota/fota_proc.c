#include "include.h"
#include "fota_proc.h"

#if AB_FOT_EN

#define FOT_DEBUG_EN                    0

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif


static fot_s fot_var;

static ab_fota_cb_t ab_fota_cb[FOTA_CHL_MAX];

static uint8_t ab_fota_chl_cur = FOTA_CHL_BLE;

static uint8_t fot_data[FOT_BLOCK_LEN] AT(.fot_data.buf);

static const uint8_t fot_auth_data[] = {0xCC, 0xAA, 0x55, 0xEE, 0x12, 0x19, 0xE4};

static uint8_t fot_reply_update_request(ab_fota_req_upgrade_t *req, uint8_t *rsp_val, uint32_t max_size);


void ab_fota_write(void *buf, u32 addr, u32 len)
{
    ota_pack_write(buf);
}

u8 ab_fota_get_err(void)
{
    return ota_pack_get_err();
}

bool ab_fota_breakpoint_info_read(void)
{
    return ota_pack_breakpoint_info_read();
}

void ab_fota_init(void)
{
    ota_pack_init();
}

u32 ab_fota_get_curaddr(void)
{
    return ota_pack_get_curaddr();
}

bool ab_fota_is_update_success(void)
{
    return ota_pack_is_update_success();
}

AT(.text.fot.update)
void ab_fota_var_init(void)
{
    u8  dev_version_str[] = SW_VERSION;
    u16 version_temp = 0;

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

void ab_fota_exit(void)
{
    if (fot_var.flag & FOT_FLAG_APP_CONNECT) {
        fot_var.sta = 0;
        fot_var.flag &= ~FOT_FLAG_APP_CONNECT;
    }
}

AT(.com_text.fot)
static u8 fot_app_connect_auth(uint8_t *packet, uint16_t size)
{
    if(!fot_var.sta){
        if((size == sizeof(fot_auth_data)) && !memcmp(fot_auth_data, packet, sizeof(fot_auth_data))){
            ab_fota_var_init();
            fot_var.flag |= FOT_FLAG_APP_CONNECT;
            return 0;
        }
    } else {
        if((size == sizeof(fot_auth_data)) && !memcmp(fot_auth_data, packet, sizeof(fot_auth_data))) {
            ab_fota_exit();
            ab_fota_var_init();
            fot_var.flag |= FOT_FLAG_APP_CONNECT;
            return 0;
        }
    }

    return fot_var.sta;
}

AT(.text.fot.cache)
static void fot_sent_proc(u8 *buf, u8 len)
{
    if ((fot_var.flag & FOT_FLAG_APP_CONNECT) == 0) {
        return;
    }

    FOT_DEBUG("fot tx:");
    FOT_DEBUG_R(buf, len);

    if (ab_fota_cb[ab_fota_chl_cur].fota_send_packet_handle) {
        ab_fota_cb[ab_fota_chl_cur].fota_send_packet_handle(buf, len);
    }
}

AT(.text.fot.update)
static void fot_reply_info_tlv(u8 *buf,u8 len)
{
    u8 read_offset = 0;
    u8 write_offset = 0;
    u8 rsp[32];
    u8 val_len = 0;

    if((buf == NULL) || (len == 0)){
        return;
    }

    rsp[write_offset++] = FOT_GET_INFO_TLV;
    rsp[write_offset++] = fot_var.seq++;

    while(read_offset < len){
        switch(buf[read_offset]){
            case INFO_DEV_VER:
                FOT_DEBUG("INFO_DEV_VER\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_VER;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_var.dev_ver & 0xff;
                rsp[write_offset++] = (fot_var.dev_ver >> 8) & 0xff;
            break;

            case INFO_UPDATE_REQ:
                FOT_DEBUG("INFO_UPDATE_REQ\n");
                {
                    ab_fota_req_upgrade_t *req;
                    val_len = buf[read_offset + 1];
                    req     = (ab_fota_req_upgrade_t *)&buf[read_offset + 2];
                    rsp[write_offset++] = INFO_UPDATE_REQ;
                    rsp[write_offset++] = 11;
                    write_offset += fot_reply_update_request(req, &rsp[write_offset], 11);
                }
            break;

            case INFO_DEV_FEATURE:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                {
                    u16 dev_ability = FOT_FEAT_DEFINITION;
                    val_len = buf[read_offset + 1];
                    rsp[write_offset++] = INFO_DEV_FEATURE;
                    rsp[write_offset++] = 2;
                    rsp[write_offset++] = dev_ability & 0xff;
                    rsp[write_offset++] = (dev_ability >> 8) & 0xff;
                }
            break;

            case INFO_DEV_CONNECT_STA:
                FOT_DEBUG("INFO_DEV_CONNECT_STA\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_CONNECT_STA;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_var.con_sta & 0xff;
                rsp[write_offset++] = (fot_var.con_sta >> 8) & 0xff;
                break;

            case INFO_PROTOCOL_VER:
                FOT_DEBUG("INFO_PROTOCOL_VER\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_PROTOCOL_VER;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = FOT_PROTOCOL_VERSION &  0xff;
                rsp[write_offset++] = FOT_PROTOCOL_VERSION >> 8;
            break;

            default:
                val_len = buf[read_offset + 1];
                break;
        }
        read_offset += (2 + val_len);
    }

    if(write_offset > sizeof(rsp)){
        printf("fot:rsp buf overflow!!!\n");
        while(1);
    }

    fot_sent_proc(rsp, write_offset);
}

AT(.text.fot.update)
static void fot_reply_dev_version(void)
{
    u8 data[5];

    data[0] = FOT_GET_INFO;
    data[1] = fot_var.seq++;
    data[2] = INFO_DEV_VER;
    memcpy(&data[3], &fot_var.dev_ver, 2);

    fot_sent_proc(data, 5);
}

AT(.text.fot.cache)
static void fot_dev_notify_sta(u8 sta)
{
    u8 buf[3];

    buf[0] = FOT_NOTIFY_STA;
    buf[1] = fot_var.seq++;
    buf[2] = sta;

    fot_sent_proc(buf, 3);
}

AT(.com_text.fot.update)
bool fota_is_start(void)
{
    return (fot_var.sta & FOT_STA_START) == FOT_STA_START;
}

AT(.text.fot.update)
void fota_pause(void)
{
    if(fot_var.sta & FOT_STA_START){
        fot_var.sta = FOT_STA_PAUSE;
        fot_dev_notify_sta(FOT_UPDATE_PAUSE);
    }
}

AT(.text.fot.update)
void fota_continue(void)
{
    if(fot_var.sta & FOT_STA_PAUSE){
        fot_var.sta = FOT_STA_INIT;
        fot_var.total_len = 0;
        fot_var.remain_len = 0;
        fot_var.data_pos = 0;
        fot_dev_notify_sta(FOT_UPDATE_CONTINUE);
    }
}

AT(.text.fot.update)
static uint8_t fot_reply_update_request(ab_fota_req_upgrade_t *req, uint8_t *rsp_val, uint32_t max_size)
{
    u16 flash_remote_ver;
    u32 hash;
    u32 addr = 0;
    u32 block_len = FOT_BLOCK_LEN;
    u16 packet_len = 0;
    u8 need_update = 1;

    if (ab_fota_cb[ab_fota_chl_cur].fota_mtu_check_handle) {
        ab_fota_cb[ab_fota_chl_cur].fota_mtu_check_handle((u32 *)&packet_len);
    } else {
        // TODO:
        printf("[FOTA] not mtu check callback\n");
    }

    if (packet_len > (FOT_BLOCK_LEN + DATA_START_POS)) {
        packet_len = FOT_BLOCK_LEN + DATA_START_POS;
    }

    printf("fot_packet_len: %d\n", packet_len);

    ab_fota_init();

    fot_var.sta |= FOT_STA_START;

    FOT_DEBUG("hash_val:0x%x\n", req->hash);

    param_fot_remote_ver_read((u8*)&flash_remote_ver);
    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);

    if ((req->hash != 0xFFFFFFFF) && (flash_remote_ver == req->version) && (hash == req->hash)) {
        if (ab_fota_breakpoint_info_read() == true) {
            addr = ab_fota_get_curaddr();
        }
    } else {
        param_fot_addr_write((u8*)&addr);
        param_fot_remote_ver_write((u8*)&req->version);
        param_fot_hash_write((u8*)&req->hash);
    }

    /* ASSERT(max_size >= 11) */

    memcpy(&rsp_val[0], &addr, 4);
    memcpy(&rsp_val[4], &block_len, 4);
    memcpy(&rsp_val[8], &packet_len, 2);
    rsp_val[10] = need_update;
    return 11;
}

//------------------------------------------------------------------------------
AT(.text.fot.cache)
void ab_fot_recv_proc(u8 *buf, u16 len)
{
    u32 addr;
    u32 recv_data_len;

    if (fot_var.remote_seq != buf[FOT_SEQ_POS]) {
        printf("remote seq err:%d,%d\n",fot_var.remote_seq, buf[FOT_SEQ_POS]);
        fot_dev_notify_sta(FOT_ERR_SEQ);
        fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
        return;
    }

    fot_var.remote_seq++;

    if ((fot_var.sta & FOT_STA_INIT) == 0) {
        return;
    }

    if (buf[FOT_CMD_POS] == FOT_GET_INFO_TLV) {
        fot_reply_info_tlv(&buf[2], len - 2);
    } else if (buf[FOT_CMD_POS] == FOT_GET_INFO) {
        if (buf[2] == INFO_DEV_VER) {
            fot_reply_dev_version();
        } else if (buf[2] == INFO_UPDATE_REQ) {
            uint8_t rsp[14];
            ab_fota_req_upgrade_t *req = (ab_fota_req_upgrade_t *)&buf[3];
            rsp[0] = FOT_GET_INFO;
            rsp[1] = fot_var.seq++;
            rsp[2] = INFO_UPDATE_REQ;
            fot_reply_update_request(req, &rsp[3], sizeof(rsp) - 3);
            fot_sent_proc(rsp, sizeof(rsp));
        }
    } else if (buf[FOT_CMD_POS] == FOT_OUT_DATA_START) {
        if (fot_var.remain_len) {
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
            fot_dev_notify_sta(FOT_ERR_DATA_LEN);
            fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            return;
        }

        recv_data_len = len - DATA_START_POS;
        memcpy(&fot_var.total_len, &buf[FOT_DATA_LEN_POS], 4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&addr, &buf[FOT_ADDR_POS], 4);
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_START_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;

        if (fot_var.remain_len == 0) {
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    } else if (buf[FOT_CMD_POS] == FOT_OUT_DATA_CONTINUE) {
        recv_data_len = len - DATA_CONTINUE_POS;
        if (fot_var.remain_len < recv_data_len) {
           recv_data_len = fot_var.remain_len;
        }
        fot_var.remain_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_CONTINUE_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;
        if (fot_var.remain_len == 0) {
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }
}

void fota_init(ab_fota_cb_t *param, uint8_t chl)
{
    printf("[MODULE] fota init\n");

    #if 0/* Sanity Check */
    if (chl > FOTA_CHL_MAX) {
        printf("Invalid fota channel !!!\n");
        WDT_RST_DELAY();
    }
    #endif

    ab_fota_cb[chl] = *param;
}

AT(.text.fot.cache)
void ab_fota_process_do(void)
{
    if (fot_var.recv_ok) {
        //FOT_DEBUG("--->fot_recv_ok\n");
        fot_var.recv_ok = 0;
        ab_fota_write(fot_data, ab_fota_get_curaddr(), fot_var.total_len);

        if (ab_fota_is_update_success()) {
            FOT_DEBUG("--->fot update success\n");
            fot_var.flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
            fot_var.tick = tick_get();
            fot_dev_notify_sta(FOT_UPDATE_DONE);
        } else {
            fot_dev_notify_sta(ab_fota_get_err());
            if (ab_fota_get_err()) {
                fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            }
        }
    }
}

AT(.text.fot.cache)
void ab_fota_flag_do(void)
{
    if (fot_var.flag & FOT_FLAG_SYS_RESET) {
        if (tick_check_expire(fot_var.tick, 3000)) {
            fot_var.flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("-->fota update ok,sys reset\n");
            WDT_RST();
        }
    }

    if (fot_var.flag & FOT_FLAG_UPDATE_EXIT){
        ab_fota_exit();
        fot_var.flag &= ~FOT_FLAG_UPDATE_EXIT;
    }
}

AT(.text.app.proc.fot)
void fota_process(void)
{
    if (fot_var.sta & FOT_STA_START) {
        lowpwr_pwroff_delay_reset();
        lowpwr_sleep_delay_reset();
        ab_fota_process_do();
    }

    if (fot_var.flag) {
        ab_fota_flag_do();
    }
}

void fota_disconnected_handle(uint8_t chl)
{
    FOT_DEBUG("--->fota_disconnect_callback: %d\n", chl);
    if (chl == ab_fota_chl_cur) {
        fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
    }
}

void fota_connected_handle(uint8_t chl)
{
    FOT_DEBUG("--->fota_connect_callback: %d\n", chl);
}

void fota_recv_packet_handle(uint8_t *buf, uint16_t size, uint8_t chl)
{
    if (fot_app_connect_auth(buf, size) && chl < FOTA_CHL_MAX) {
        ab_fota_chl_cur = chl;
        ab_fot_recv_proc(buf, size);
    }
}

#endif // AB_FOT_EN
