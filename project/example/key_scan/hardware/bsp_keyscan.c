#include "include.h"
#include "bsp_keyscan.h"
#include "driver_keyscan.h"
#include "driver_gpio.h"
#include "key_scan.h"

struct{
    uint32_t key_valA;
    uint32_t key_valB;
}keyscan_cb;

AT(.com_text.keyscan.isr)
void keyscan_isr_handler(void)
{
    if (keyscan_get_flag(KEYSCAN_IT_VALID) != RESET) {
        keyscan_cb.key_valA = keyscan_get_groupA_key_id();
        keyscan_cb.key_valB = keyscan_get_groupB_key_id();
        keyscan_clear_flag(KEYSCAN_IT_VALID);
#if SYS_SLEEP_LEVEL
        if(sys_cb.sleep_sta != LOWPWR_SLEEP_EXIT){
            sys_cb.sleep_prevent = true;
        }
        lowpwr_sleep_delay_reset();
#endif
    }

    if (keyscan_get_flag(KEYSCAN_IT_INVALID) != RESET) {
        keyscan_cb.key_valA = 0;
        keyscan_cb.key_valB = 0;
        keyscan_clear_flag(KEYSCAN_IT_INVALID);
    }
}

void bsp_keyscan_init(void)
{
    keyscan_init_typedef keyscan_init_struct;

    memset(&keyscan_cb, 0, sizeof(keyscan_cb));

    clk_gate0_cmd(CLK_GATE0_LP, CLK_EN);
    clk_clk2m_ks_set(CLK_CLK2M_KS_SNF_RC_RTC);

    keyscan_init_struct.key_mask = 0xff;         //High~Low：PB9~PB5, PB2~PB0
    keyscan_init_struct.stable_times = 255;
    keyscan_init_struct.debounce_times = 0x05;
    keyscan_init_struct.wait_times = 0x05;
    keyscan_init_struct.key_mode = KEYSCAN_MODE_MATRIX;
    keyscan_init_struct.sw_reset = KEYSCAN_SWRSTN_RELEASE_RESET;
    keyscan_init_struct.low_power_mode = KEYSCAN_LOWPWR_DIS;
    keyscan_init_struct.wakeup = KEYSCAN_WKUP_EN;
    keyscan_init(&keyscan_init_struct);

    keyscan_pic_config(keyscan_isr_handler, 0, KEYSCAN_IT_VALID | KEYSCAN_IT_INVALID, ENABLE);

    keyscan_cmd(ENABLE);
}


AT(.com_text.key_scan)
uint8_t bsp_get_keyscan_id(void)
{
    uint8_t key_id = KEY_ID_NO;

    switch (keyscan_cb.key_valA) {
        case KA1:
            key_id = KEY_ID_K1;
            break;
        case KA2:
            key_id = KEY_ID_K2;
            break;
        case KA3:
            key_id = KEY_ID_K3;
            break;
        case KA4:
            key_id = KEY_ID_K4;
            break;
        case KA5:
            key_id = KEY_ID_K5;
            break;
        case KA6:
            key_id = KEY_ID_K6;
            break;
        case KA7:
            key_id = KEY_ID_K7;
            break;
        case KB2:
            key_id = KEY_ID_K8;
            break;
        case KB3:
            key_id = KEY_ID_K9;
            break;
        case KB4:
            key_id = KEY_ID_K10;
            break;
        case KB5:
            key_id = KEY_ID_K11;
            break;
        case KB6:
            key_id = KEY_ID_K12;
            break;
        case KB7:
            key_id = KEY_ID_K13;
            break;
        case KC3:
            key_id = KEY_ID_K14;
            break;
        case KC4:
            key_id = KEY_ID_K15;
            break;
        case KC5:
            key_id = KEY_ID_K16;
            break;
        case KC6:
            key_id = KEY_ID_K17;
            break;
        case KC7:
            key_id = KEY_ID_K18;
            break;
        case KD4:
            key_id = KEY_ID_K19;
            break;
        case KD5:
            key_id = KEY_ID_K20;
            break;
        case KD6:
            key_id = KEY_ID_K21;
            break;
        case KD7:
            key_id = KEY_ID_K22;
            break;
        case KE5:
            key_id = KEY_ID_K23;
            break;
        case KE6:
            key_id = KEY_ID_K24;
            break;
        case KE7:
            key_id = KEY_ID_K25;
            break;
        case KF6:
            key_id = KEY_ID_K26;
            break;
        case KF7:
            key_id = KEY_ID_K27;
            break;
        case KG7:
            key_id = KEY_ID_K28;
            break;
        default:
            break;
    }

    switch (keyscan_cb.key_valB) {
        case KJ0:
            key_id = KEY_ID_K29;
            break;
        case KJ1:
            key_id = KEY_ID_K30;
            break;
        case KJ2:
            key_id = KEY_ID_K31;
            break;
        case KJ3:
            key_id = KEY_ID_K32;
            break;
        case KJ4:
            key_id = KEY_ID_K33;
            break;
        case KJ5:
            key_id = KEY_ID_K34;
            break;
        case KJ6:
            key_id = KEY_ID_K35;
            break;
        case KJ7:
            key_id = KEY_ID_K36;
            break;
        case KI0:
            key_id = KEY_ID_K37;
            break;
        case KI1:
            key_id = KEY_ID_K38;
            break;
        case KI2:
            key_id = KEY_ID_K39;
            break;
        case KI3:
            key_id = KEY_ID_K40;
            break;
        case KI4:
            key_id = KEY_ID_K41;
            break;
        case KI5:
            key_id = KEY_ID_K42;
            break;
        case KI6:
            key_id = KEY_ID_K43;
            break;
        case KI7:
            key_id = KEY_ID_K44;
            break;
        default:
            break;
    }

    return key_id;
}
