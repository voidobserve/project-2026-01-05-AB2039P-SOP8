#include "include.h"
#include "driver_com.h"
#include "driver_saradc.h"
#include "driver_gpio.h"
#include "usb_audio.h"

#if NOTCH_IIR_EN
#include "notch_iir.h"
#endif

#if BSP_SARADC_MIC_EN

#define SARADC_MIC_SPR        SARADC_MIC_MIC_SR_16K
#define SARADC_MIC_BUF_LEN    16


int16_t saradc_mic_dma_buf[SARADC_MIC_BUF_LEN];

AT(.com_text.mic.isr)
void saradc_mic_ch_isr(void)
{
    if (saradc_get_flag(SARADC_MIC_DMA_ALL_DONE) != RESET) {
        saradc_clear_flag(SARADC_MIC_DMA_ALL_DONE);
        for (int i = (SARADC_MIC_BUF_LEN/2); i<(SARADC_MIC_BUF_LEN); i++) {
            audio_mic_dc_filter(&saradc_mic_dma_buf[i]);
        }
        #if NOTCH_IIR_EN
        notch_iir_process(&saradc_mic_dma_buf[SARADC_MIC_BUF_LEN/2]);
        #endif  //NOTCH_IIR_EN

        usb_audio_mic_stream(&saradc_mic_dma_buf[SARADC_MIC_BUF_LEN/2], SARADC_MIC_BUF_LEN/2);

    } else if (saradc_get_flag(SARADC_MIC_DMA_HALF_DONE) != RESET) {
        saradc_clear_flag(SARADC_MIC_DMA_HALF_DONE);
        for (int i = 0; i<(SARADC_MIC_BUF_LEN/2); i++) {
            audio_mic_dc_filter(&saradc_mic_dma_buf[i]);
        }
        #if NOTCH_IIR_EN
        notch_iir_process(&saradc_mic_dma_buf[0]);
        #endif  //NOTCH_IIR_EN
        usb_audio_mic_stream(saradc_mic_dma_buf, SARADC_MIC_BUF_LEN/2);
    }
}

void bsp_saradc_mic_init(void)
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

    /********************************saradc start*******************************/
    saradc_cmd(ENABLE);
}

void bsp_saradc_mic_start(void)
{
    #if NOTCH_IIR_EN
    notch_iir_init(SARADC_MIC_SPR);
    #endif  //NOTCH_IIR_EN
    audio_mic_dc_filter_init();
    saradc_mic_dma_cmd((uint32_t)saradc_mic_dma_buf, SARADC_MIC_BUF_LEN, ENABLE);
    saradc_mic_cmd(ENABLE);
}

void bsp_saradc_mic_stop(void)
{
    saradc_mic_dma_cmd((uint32_t)saradc_mic_dma_buf, SARADC_MIC_BUF_LEN, DISABLE);
    saradc_mic_cmd(DISABLE);
}

#endif  //BSP_SARADC_MIC_EN
