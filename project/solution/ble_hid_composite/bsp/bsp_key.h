#ifndef _BSP_KEY_H
#define _BSP_KEY_H


// Key Property
#define KEY_SCAN_TIMES          6           //按键防抖的扫描次数
#define KEY_UP_TIMES            6           //抬键防抖的扫描次数
#define KEY_LONG_TIMES          50         //长按键的次数
#define KEY_HOLD_TIMES          15          //连按的频率次数

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05
#define KEY_ID_K5               0x06
#define KEY_ID_K6               0x07


void bsp_key_init(void);
u16 bsp_key_scan(u8 mode);
void bsp_key_set_power_on_flag(bool flag);


#endif // _BSP_KEY_H
