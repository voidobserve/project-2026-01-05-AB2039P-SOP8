#include "include.h"
#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "bsp_saradc.h"

#if NOTCH_IIR_EN
#include "notch_iir.h"
#endif

#if THR_TRACK_FLASH_EN
#include "thr_track_flash.h"
#endif

#if BSP_MIC_EN

#define SARADC_MIC_SPR        SARADC_MIC_MIC_SR_8K
#define SARADC_MIC_BUF_LEN    256

enum {
    MIC_DMA_DONE_TYPE_NONE = 0,
    MIC_DMA_ALL_DONE,
    MIC_DMA_HALF_DONE,
};

static bool saradc_mic_start_flag = 0;
static volatile bool saradc_mic_drumbeat_flag;
static volatile uint8_t saradc_mic_dma_done_type = MIC_DMA_DONE_TYPE_NONE;
int16_t saradc_mic_dma_buf[SARADC_MIC_BUF_LEN] AT(.buf.saradc_mic);

//获取鼓点标志
bool saradc_mic_drumbeat_flag_get(void)
{
    return saradc_mic_drumbeat_flag;
}

void saradc_mic_ch_drumbeat_proc(void)
{
    if (saradc_mic_drumbeat_flag) {
        printf("drum\n");
        saradc_mic_drumbeat_flag = 0;
    }
}

AT(.com_text.mic.isr)
void saradc_mic_ch_isr(void)
{
    if (saradc_get_flag(SARADC_MIC_DMA_ALL_DONE) != RESET) {
        saradc_clear_flag(SARADC_MIC_DMA_ALL_DONE);
        saradc_mic_dma_done_type = MIC_DMA_ALL_DONE;
    } else if (saradc_get_flag(SARADC_MIC_DMA_HALF_DONE) != RESET) {
        saradc_clear_flag(SARADC_MIC_DMA_HALF_DONE);
        saradc_mic_dma_done_type = MIC_DMA_HALF_DONE;
    }
}

void saradc_mic_ch_init(void)
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

    /********************************mic channel init*******************************/
    sdadc_mic_init_typedef saradc_mic_init_struct;
    saradc_mic_init_struct.mask_length = 0;
    saradc_mic_init_struct.sample_rate = SARADC_MIC_SPR;
    saradc_mic_init_struct.mic_rc = AUDIO_MIC_NOT_RC;
    saradc_mic_init_struct.mic_vdd = AUDIO_MIC_VDD_2V0;
    saradc_mic_init_struct.mic_bias_res =  AUDIO_MIC_BIAS_RES_4K;
    saradc_mic_init_struct.mic_again = AUDIO_MIC_ANA_GAIN_7;
    saradc_mic_init_struct.micin_trim = 1400;  //micin trim target value, uint:mV, AUDIO_MIC_NOT_RC and AUDIO_MIC_NOT_R valid
    saradc_mic_init(&saradc_mic_init_struct);

    if ((saradc_mic_init_struct.mic_vdd != AUDIO_MIC_VDD_NONE)  && \
        (saradc_mic_init_struct.mic_rc != AUDIO_MIC_NOT_RC)) {
        GPIOBDE &= ~BIT(8);     //mic bias PB8
    }

    saradc_pic_config(saradc_mic_ch_isr, 0, SARADC_MIC_DMA_HALF_DONE | SARADC_MIC_DMA_ALL_DONE, ENABLE);

    // /********************************saradc start*******************************/
    // saradc_cmd(ENABLE);

    // /********************************mic dma start*******************************/
    // saradc_mic_ch_start();
}


void saradc_mic_ch_start(void)
{
    #if THR_TRACK_FLASH_EN
        thr_track_flash_init();
    #endif //THR_TRACK_FLASH_EN

    #if NOTCH_IIR_EN
        notch_iir_init(SARADC_MIC_SPR);
    #endif  //NOTCH_IIR_EN
    audio_mic_dc_filter_init();
    saradc_cmd(ENABLE);
    saradc_mic_dma_cmd((uint32_t)saradc_mic_dma_buf, SARADC_MIC_BUF_LEN, ENABLE);
    saradc_mic_cmd(ENABLE);
}

void saradc_mic_ch_stop(void)
{
    saradc_mic_start_flag = 0;
    saradc_mic_dma_cmd((uint32_t)saradc_mic_dma_buf, SARADC_MIC_BUF_LEN, DISABLE);
    saradc_mic_cmd(DISABLE);
}


void saradc_mic_ch_data_proc(void)
{
    if (saradc_mic_dma_done_type == MIC_DMA_ALL_DONE) {
        saradc_mic_dma_done_type = MIC_DMA_DONE_TYPE_NONE;
        for (int i = (SARADC_MIC_BUF_LEN/2); i<(SARADC_MIC_BUF_LEN); i++) {
            audio_mic_dc_filter(&saradc_mic_dma_buf[i]);
        }

#if NOTCH_IIR_EN
        notch_iir_process(&saradc_mic_dma_buf[SARADC_MIC_BUF_LEN/2]);
#endif  //NOTCH_IIR_EN

#if THR_TRACK_FLASH_EN
        saradc_mic_drumbeat_flag = thr_track_flash_process(&saradc_mic_dma_buf[SARADC_MIC_BUF_LEN/2]);
#endif //THR_TRACK_FLASH_EN
    } else if (saradc_mic_dma_done_type == MIC_DMA_HALF_DONE) {
        saradc_mic_dma_done_type = MIC_DMA_DONE_TYPE_NONE;
        for (int i = 0; i<(SARADC_MIC_BUF_LEN/2); i++) {
            audio_mic_dc_filter(&saradc_mic_dma_buf[i]);
        }

#if NOTCH_IIR_EN
        notch_iir_process(&saradc_mic_dma_buf[0]);
#endif  //NOTCH_IIR_EN

#if THR_TRACK_FLASH_EN
        saradc_mic_drumbeat_flag = thr_track_flash_process(&saradc_mic_dma_buf[0]);
#endif //THR_TRACK_FLASH_EN
    }
}

void saradc_mic_ch_proc(void)
{
    audio_mic_trim_proc();  //底层trim完不会再执行

    if (audio_mic_trim_is_finish()) {

        if (saradc_mic_start_flag == 0) {
            saradc_mic_start_flag = 1;
            saradc_mic_ch_start();
        }

        saradc_mic_ch_data_proc();
        saradc_mic_ch_drumbeat_proc();
    }
}

#endif  //BSP_MIC_EN
