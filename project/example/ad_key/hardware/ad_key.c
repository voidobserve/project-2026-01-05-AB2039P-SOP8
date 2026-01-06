#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "ad_key.h"


// The port and pin should be set according to adc channel.
#define ADC_CHANNEL         ADC_CHANNEL_PB2
#define ADC_PORT            GPIOB_REG
#define ADC_PIN             GPIO_PIN_2

// If the key table does not match the actual value, adjust it according to the actual situation.
// Note: When you calibrate key values, it is recommended to overlay a positive value on top of the actual key value,
// e.p.: (or see Line 105)
//      KEY_PP_DOWN -->   adc_val=0x00(actual)  -->  0x05(write to table)
#define KEY_VALUE_DEBUG     0


const key_val_mapping_typedef key_val_mapping_table[6] = {
		{KEY_VALUE_PP,      KEY_ID_PP},
		{KEY_VALUE_KEY1,    KEY_ID_K1},
		{KEY_VALUE_KEY2,    KEY_ID_K2},
		{KEY_VALUE_KEY3,    KEY_ID_K3},
		{KEY_VALUE_KEY4,    KEY_ID_K4},
		{KEY_VALUE_NULL,    KEY_ID_NO},
};

u16 allow_multiple_key_table[] = {
    KEY_ID_PP | KEY_SHORT_UP,
    KEY_ID_K1 | KEY_SHORT_UP,
};

static key_status_typedef key_status;

// 初始化ADC按键
void ad_key_init(void)
{
    saradc_base_init_typedef saradc_base_init_struct;

    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_EN);
    clk_saradc_clk_set(CLK_SARADC_CLK_X24M_CLKDIV4);

    saradc_base_init_struct.mode = SARADC_MODE_NORMAL;
    saradc_base_init_struct.baud = 1500000;
    saradc_base_init_struct.auto_analog = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_base_init_struct.auto_analog_io = SARADC_AUTO_ENABLE_ANALOG_IO_EN;
    saradc_init(&saradc_base_init_struct);

    saradc_cmd(ENABLE);

    saradc_add_channel(ADC_CHANNEL, SARADC_PULLUP_DIS);
    saradc_kick_start();

    memset(&key_status, 0, sizeof(key_status));
    multiple_key_init(60, allow_multiple_key_table, sizeof(allow_multiple_key_table) / sizeof(u16));
}


// 处理短按、长按、连按
u16 key_process(u16 key_id)
{
    u16 key_return = KEY_ID_NO;

    if (key_id == KEY_ID_NO || key_id != key_status.key_id) {               // 抬键消抖
        if (key_status.release_debounce_cnt < KEY_UP_TIMES) {
            key_status.release_debounce_cnt++;
        } else {
            if (key_status.key_press_cnt >= KEY_LONG_TIMES) {               // 长按抬键
                key_return = key_status.key_id | KEY_LONG_UP;
            } else if (key_status.key_press_cnt >= KEY_SCAN_TIMES) {        // 短按抬键
                key_return = key_status.key_id | KEY_SHORT_UP;
            }

            key_status.key_id = key_id;
            key_status.key_press_cnt = 0;
        }
    } else {
        key_status.key_press_cnt++;
        if (key_status.key_press_cnt == KEY_SCAN_TIMES) {               // 去抖
            key_status.release_debounce_cnt = 0;
            key_return = key_status.key_id;
        } else if (key_status.key_press_cnt == KEY_LONG_TIMES) {        // 长按
            key_return = key_status.key_id | KEY_LONG;
        } else if (key_status.key_press_cnt == KEY_LONG_HOLD_TIMES) {   // 连按
            key_return = key_status.key_id | KEY_HOLD;
            key_status.key_press_cnt = KEY_LONG_TIMES;
        }
    }

    return key_return;
}

// 读取按键的ADC采样值并根据数值返回键值
static u8 get_key_id(void)
{
    u8 num = 0;

    u16 key_adc_val = saradc_get_channel_value(ADC_CHANNEL);

#if KEY_VALUE_DEBUG
    printf("---> adc value: 0x%04x\n", key_adc_val + 0x05);
#endif // KEY_VALUE_DEBUG

    while (key_adc_val > key_val_mapping_table[num].adc_val) {
        num++;
    }

    return key_val_mapping_table[num].usage_id;
}

// 测试函数
void ad_key_example(void)
{
    static u32 delay_cnt;
    u16 key, key_id;

    if (!tick_check_expire(delay_cnt, 5)) {
        return;
    }
    delay_cnt = tick_get();

    if (saradc_get_flag(SARADC_IT_FINISHED)) {
        saradc_clear_flag(SARADC_IT_FINISHED);
        saradc_get_result();
        saradc_kick_start();
    }

    key_id = get_key_id();                  // 根据ADC采样值返回键值
    key = key_process(key_id);              // 短按、长按、连按处理
    key = key_multi_press_process(key);     // 多击处理

    switch (key) {
        case KEY_ID_PP | KEY_SHORT:         // PP按键短按
            printf("PP key short down\n");
            break;

        case KEY_ID_PP | KEY_SHORT_UP:      // PP按键短按抬起
            printf("PP key short key up\n");
            break;

        case KEY_ID_PP | KEY_LONG:          // PP长按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_HOLD:          // PP连按
            printf("PP key long down\n");
            break;

        case KEY_ID_PP | KEY_LONG_UP:       // PP长按抬起
            printf("PP key long up\n");
            break;

        case KEY_ID_PP | KEY_DOUBLE:        // PP双击
            printf("PP key double down\n");
            break;

        case KEY_ID_PP | KEY_THREE:         // PP三击
            printf("PP key triple down\n");
            break;

        case KEY_ID_K1 | KEY_SHORT:         // K1短按
            printf("K1 key short down\n");
            break;

        default:
            if (key != KEY_ID_NO) {
                printf("key event: %X\n", key);
            }
            break;
    }
}
