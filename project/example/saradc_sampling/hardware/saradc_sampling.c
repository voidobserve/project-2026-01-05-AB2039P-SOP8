#include "include.h"
#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "saradc_sampling.h"
#include "saradc_mic_channel.h"
#include "saradc_general_channel.h"

void saradc_sampling_init(void)
{
    /********************************saradc base init*******************************/
    saradc_base_init_typedef saradc_base_init_struct;

    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_EN);
    clk_saradc_clk_set(CLK_SARADC_CLK_X24M_CLKDIV2);

    saradc_base_init_struct.mode = SARADC_MODE_NORMAL;
    saradc_base_init_struct.baud = 1500000;
    saradc_base_init_struct.auto_analog = SARADC_AUTO_ENABLE_ANALOG_EN;
    saradc_base_init_struct.auto_analog_io = SARADC_AUTO_ENABLE_ANALOG_IO_EN;
    saradc_init(&saradc_base_init_struct);

#if SARADC_MIC_EN
    /********************************mic channel init*******************************/
    saradc_mic_ch_init();
#endif  //SARADC_MIC_EN

    /********************************saradc start*******************************/
    saradc_cmd(ENABLE);

#if SARADC_GENERAL_EN
    /********************************general channel add*******************************/
    saradc_general_ch_add_ch_test_0();
    saradc_general_ch_add_ch_test_1();
#endif //SARADC_GENERAL_EN

#if SARADC_MIC_EN
    /********************************mic dma start*******************************/
    saradc_mic_ch_start();
#endif  //SARADC_MIC_EN
}

void saradc_sampling_run(void)
{
    static uint32_t tick = -1;

    if (tick_check_expire(tick, 50)) {
        tick = tick_get();
#if SARADC_GENERAL_EN
        saradc_general_ch_proc();
        saradc_general_ch_user_proc();
#endif  // SARADC_GENERAL_EN
    }

#if SARADC_MIC_EN
    saradc_mic_ch_proc();
#endif  // SARADC_MIC_EN
}
