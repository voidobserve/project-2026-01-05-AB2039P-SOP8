#ifndef _BSP_KEY_H
#define _BSP_KEY_H


// Key Property
#define KEY_SCAN_TIMES          3             //按键防抖的扫描次数
#define KEY_UP_TIMES            6             //抬键防抖的扫描次数
#define KEY_LONG_TIMES          150           //长按键的次数
#define KEY_HOLD_TIMES          35            //连按的频率次数
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05
#define KEY_ID_K5               0x06
#define KEY_ID_K6               0x07
#define KEY_ID_K7               0x08
#define KEY_ID_K8               0x09
#define KEY_ID_K9               0x0a
#define KEY_ID_K10              0x0b
#define KEY_ID_K11              0x0c
#define KEY_ID_K12              0x0d
#define KEY_ID_K13              0x0e

typedef enum {
    KEY_MSG_RELEASE             = 0x00,  //按键抬起
    KEY_MSG_UP                  = 0x01,
    KEY_MSG_DOWN                = 0x02,
    KEY_MSG_LEFT                = 0x03,
    KEY_MSG_RIGHT               = 0x04,
    KEY_MSG_MEUN                = 0x05,
    KEY_MSG_HOME                = 0x06,
    KEY_MSG_OK                  = 0x07,
    KEY_MSG_BACK                = 0x08,
    KEY_MSG_VOL_UP              = 0x09,
    KEY_MSG_VOL_DOWN            = 0x0a,
    KEY_MSG_POWER               = 0x0b,
    KEY_MSG_VOICE_ON            = 0x0c,  //开始语音
    KEY_MSG_VOICE_OFF           = 0x0d,  //结束语音
    KEY_MSG_PAIR                = 0x0e,
    KEY_MSG_NONE                = 0xff,
} KEY_MSG_TYPEDEF;  //范围：0x00~0xff

typedef struct {
    u16 key_id;
    u16 key_press_cnt;
    u8 release_debounce_cnt;
    u8 power_on_flag;
} key_status_typedef;

void bsp_key_init(void);
u16 bsp_key_scan(u8 mode);
void bsp_key_set_power_on_flag(bool flag);
bool bsp_key_get_power_on_flag(void);
void bsp_key_set_msg(KEY_MSG_TYPEDEF msg);
KEY_MSG_TYPEDEF bsp_key_get_msg(void);

#endif // _BSP_KEY_H
