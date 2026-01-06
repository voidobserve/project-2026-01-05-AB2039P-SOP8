#ifndef _AD_KEY_H
#define _AD_KEY_H


// Key Property
#define KEY_SCAN_TIMES          6           //按键防抖的扫描次数
#define KEY_UP_TIMES            6           //抬键防抖的扫描次数
#define KEY_LONG_TIMES          150         //长按键的次数
#define KEY_HOLD_TIMES          35          //连按的频率次数
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05

// Key ADC Value (AB202X DEV V1.0)
#define KEY_VALUE_PP            0x0005
#define KEY_VALUE_KEY1          0x0061
#define KEY_VALUE_KEY2          0x00ae
#define KEY_VALUE_KEY3          0x0104
#define KEY_VALUE_KEY4          0x015c
#define KEY_VALUE_NULL          0xffff

typedef struct {
    u16 key_id;
    u16 key_press_cnt;
    u8 release_debounce_cnt;
} key_status_typedef;

typedef struct {
    u16 adc_val;
    u8 usage_id;
} key_val_mapping_typedef;

void ad_key_init(void);
void ad_key_example(void);

#endif // _AD_KEY_H
