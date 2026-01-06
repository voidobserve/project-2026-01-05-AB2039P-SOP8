#include "key_scan.h"
#include "bsp_keyscan.h"


void key_scan_init(void)
{
    bsp_keyscan_init();
}

void key_scan_example(void)
{
    uint8_t curr_key_id = KEY_ID_NO;
    static uint8_t last_key_id = KEY_ID_NO;
    static uint32_t tick = -1;
    if (tick_check_expire(tick, 5)) {
        tick = tick_get();
        curr_key_id = bsp_get_keyscan_id();
        if (curr_key_id != last_key_id) {
            last_key_id = curr_key_id;
            printf("key id:%d\n", curr_key_id);
        }
    }
}
