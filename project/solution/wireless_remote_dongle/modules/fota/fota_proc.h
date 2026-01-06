#ifndef _FOTA_PROC_H
#define _FOTA_PROC_H


#define FOT_STA_INIT                    BIT(0)
#define FOT_STA_START                   BIT(1)
#define FOT_STA_PAUSE                   BIT(2)

#define FOT_FLAG_UPDATE_OK              BIT(0)
#define FOT_FLAG_SYS_RESET              BIT(1)
#define FOT_FLAG_APP_CONNECT            BIT(2)
#define FOT_FLAG_CLK_SET                BIT(3)
#define FOT_FLAG_UPDATE_EXIT            BIT(4)

#define FOT_NOTIFY_STA                  0x90
#define FOT_GET_INFO                    0x91
#define FOT_GET_INFO_TLV                0x92
#define FOT_OUT_DATA_START              0xA0
#define FOT_OUT_DATA_CONTINUE           0x20

#define FOT_CMD_POS                     0
#define FOT_SEQ_POS                     1
#define FOT_ADDR_POS                    2
#define FOT_DATA_LEN_POS                6
#define DATA_START_POS                  10
#define DATA_CONTINUE_POS               2

#define FOT_BLOCK_LEN                   512

/* Format: Ver. 2.3 => 0x0203  */
#define FOT_PROTOCOL_VERSION            0x0200

/* Unsigned Integer: 2Bytes */
#define FOT_FEAT_DEFINITION             ((0/* TWS */        << 0)|\
                                         (AB_FOT_EN         << 1)|\
                                         (0/* Other */      << 15))

typedef enum {
    FOTA_CHL_BLE,
    FOTA_CHL_USB,
    FOTA_CHL_MAX,
} FOTA_CHL_TYPEDEF;

typedef enum {
    INFO_DEV_VER                        = 1,
    INFO_UPDATE_REQ,
    INFO_DEV_FEATURE,
    INFO_DEV_CONNECT_STA,
    INFO_EDR_ADDR,
    INFO_DEV_CHANNEL,
    INFO_PROTOCOL_VER,
} DEV_INFO_E;

typedef struct PACKED {
    uint8_t     seq;
    uint8_t     remote_seq;
    uint8_t     sta;
    uint8_t     flag;
    uint16_t    con_sta;
    uint16_t    dev_ver;
    uint16_t    remote_ver;
    uint32_t    total_len;
    uint32_t    remain_len;
    uint32_t    hash;
    uint16_t    data_pos;
    uint8_t     recv_ok;
    uint32_t    addr;
    uint32_t    tick;
} fot_s;

typedef struct {
    int (* fota_send_packet_handle) (uint8_t *buf, uint16_t size);
    int (* fota_mtu_check_handle)   (uint32_t *mtu);
} ab_fota_cb_t;

typedef struct PACKED {
    uint16_t    version;
    uint32_t    hash;
    uint32_t    fw_size;
} ab_fota_req_upgrade_t;


void fota_init(ab_fota_cb_t *param, uint8_t chl);
void fota_process(void);

void fota_connected_handle(uint8_t chl);
void fota_disconnected_handle(uint8_t chl);
void fota_recv_packet_handle(uint8_t *buf, uint16_t size, uint8_t chl);

void fota_pause(void);
void fota_continue(void);
bool fota_is_start(void);

#endif // _FOTA_PROC_H
