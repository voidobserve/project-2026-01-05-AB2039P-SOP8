#include "include.h"
#include "driver_saradc.h"
#include "bsp_saradc_vbat.h"

#if BSP_VBAT_DETECT_EN

#define VBAT_FILTER_BUF_LEN  5

static u32 vbat_voltage = 0;

// 电池电量相关参数
const vbat_param_struct vbat_param = {
    .vbat_max_level = 100,
    .vbat_min_power = 2500,
    .vbat_max_power = 4200,
};

// 初始化Vbat检测
void bsp_vbat_detect_init(void)
{
    saradc_add_channel(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL, SARADC_PULLUP_DIS);
    saradc_kick_start();
}

uint32_t bsp_vbat_filter(uint32_t voltage)
{
    static uint8_t div = 1;
    static uint8_t index = 0;
    static uint32_t filter_buf[VBAT_FILTER_BUF_LEN] = {0};
    uint32_t total_vol = 0;
    uint32_t result = 0;

    filter_buf[index] = voltage;
    for (uint8_t i=0; i<VBAT_FILTER_BUF_LEN; i++) {
        total_vol += filter_buf[i];
    }
    result =  total_vol / div;
    div = (div >= VBAT_FILTER_BUF_LEN) ? VBAT_FILTER_BUF_LEN : (div+1);
    index = (index >= (VBAT_FILTER_BUF_LEN-1)) ? 0 : (index+1);

    return result;
}

AT(.text.app.proc.vbat)
void bsp_vbat_process(void)
{
    uint16_t diff;
    uint32_t adc_val;
    uint32_t adc_vbg;
    uint32_t vbat_cur;
    static uint32_t tick = -1;
    if (tick_check_expire(tick, 500)) {
        tick = tick_get();
        adc_val = saradc_get_channel_value(ADC_CHANNEL_VBAT);
        adc_vbg = saradc_get_channel_value(VBAT_DETECT_ASSIST_CHANNEL);
        if (adc_vbg) {
            vbat_cur = adc_val * VBAT_DETECT_ASSIST_VOLTAGE / adc_vbg;
            vbat_cur = bsp_vbat_filter(vbat_cur);
            if(vbat_voltage > vbat_cur) {
                diff = vbat_voltage - vbat_cur;
            } else {
                diff = vbat_cur - vbat_voltage;
            }
            if (diff >= 30) {
                vbat_voltage = vbat_cur;
            }
        }
    }
}

// 获取电池电量： 1：电量adc值(mA)  0：电量水平
uint16_t bsp_get_vbat_level(u8 mode)
{
    uint8_t current_power;
    if (mode == 0) {
        if (vbat_param.vbat_min_power > vbat_voltage) {
            return 0;
        }
        current_power = vbat_param.vbat_max_level * \
        (vbat_voltage - vbat_param.vbat_min_power) / (vbat_param.vbat_max_power - vbat_param.vbat_min_power);
        current_power = (current_power > vbat_param.vbat_max_level) ? vbat_param.vbat_max_level : current_power;
        return current_power;
    } else {
        return vbat_voltage;
    }
}

#endif // BSP_VBAT_DETECT_EN
