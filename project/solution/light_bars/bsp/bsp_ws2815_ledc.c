#include "include.h"
#include "driver_gpio.h"
#if BSP_IR_EN
#include "driver_ir.h"
#endif

#if (BSP_LEDC_EN)
#include "driver_ledc.h"
#include "driver_tmr.h"

#define LEDC_OUTPUT_PORT  GPIOB_REG
#define LEDC_OUTPUT_PIN   GPIO_PIN_1

uint32_t conversion_color = 0;
AT(.data.ledc)
uint32_t color_buf[LEDC_FRAME_LEN] = {0};
AT(.ledc_buf)
uint32_t code_buf[LEDC_RGB_CODE_LEN] = {0};

#if DOUBLE_BUFFER_SWITCH
AT(.ledc_buf)
uint32_t code_buf1[LEDC_RGB_CODE_LEN] = {0};
AT(.ledc_buf)
volatile unsigned char switch_buf_state = 0;
#endif

volatile unsigned int timer_count = 0;

volatile unsigned char fill_state = 0;
volatile unsigned char ledc_kick_state = 0;
volatile uint32_t value = 0;

static uint8_t bsp_ws2815_app_en = 0;

uint32_t sysclk_sel_tbl[] = {2000, 16000, 24000, 48000, 60000, 80000, 120000, 160000};

void bsp_ledc_start(void)
{
    ledc_cmd(ENABLE);
}

void bsp_ledc_stop(void)
{
    ledc_cmd(DISABLE);
}

void bsp_ws2815_app_set(uint8_t enable)
{
    bsp_ws2815_app_en = enable;

    printf("bsp_ws2815_app_en %x\n", bsp_ws2815_app_en);

    #if 0
    if (enable) {
        bsp_ledc_start();
    }else {
        bsp_ledc_stop();
    }
    #endif
}

/***********************Hardware Init***************************/
AT(.com_text.isr)
void bsp_basetimer_isr(void)
{
    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        timer_count++;

        if((timer_count >= DMA_TICK_INTERVAL) && (ledc_kick_state == 1)){
            timer_count = 0;
            ledc_kick();
        }
    }
}

void bsp_basetimer_init(void)
{
    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    tmr_base_init_typedef tmr_base_init_struct;

    tmr_base_init_struct.tick_en = DISABLE;
    tmr_base_init_struct.period = 250 - 1;                    // 1ms (1s == 250000)
    tmr_base_init_struct.tmr_mode = TMR_TIMER_MODE;           // 计数
    tmr_base_init_struct.clock_div = TMR_CLOCK_DIV_4;         // 4分频
    tmr_base_init_struct.clock_source = TMR_XOSC_CLKDIV24;    // 1MHz
    tmr_base_init(TMR3, &tmr_base_init_struct);

    tmr_pic_config(TMR3, bsp_basetimer_isr, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR3, ENABLE);
}

////注意：bsp_ledc_isr是高优先级中断服务函数，不能在该函数中加太多处理代码，以免影响其他代码的执行！！
AT(.com_text.isr) FIQ
void bsp_ledc_isr(void)
{
    //we should clear pending flag no matter which pending comes.
    //if (ledc_get_flag(LEDC_FLAG_DMA)) {
    if (1) {
        if (ledc_get_flag(LEDC_FLAG_DONE)) {
            ledc_clear_flag(LEDC_FLAG_DONE);
            ledc_kick_state = 0;
        }

        if (ledc_get_flag(LEDC_FLAG_RST)) {
            ledc_clear_flag(LEDC_FLAG_RST);
        }

        if (ledc_get_flag(LEDC_FLAG_DMA)) {
            ledc_clear_flag(LEDC_FLAG_DMA);

            #if (SAVE_MEMORY_ENABLE)
            #if (DOUBLE_BUFFER_SWITCH)

            if (switch_buf_state == 0) {
                ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);
            }else if (switch_buf_state == 1) {
                ledc_dma_kick((u32)&code_buf1[0], LEDC_RGB_CODE_LEN);
            }
            #else

            ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);

            #endif//#if DOUBLE_BUFFER_SWITCH
            #endif//#if SAVE_MEMORY_ENABLE
        }
    }
}

AT(.com_text.irq)
void bsp_ledc_irq(void)     //解码中断
{
    //#if (BSP_IR_EN)
    //bsp_irrx_irq();
    //#endif

    bsp_ledc_isr();
}

/*************************ws2815 software application******************************/
static uint32_t ws2815_rgb_color_format_conversion(uint32_t display_color,
                                                unsigned char display_format)
{
    uint32_t color_temp = 0;

    switch (display_format) {
        case LEDC_RGB://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 8);
            display_color |= ((color_temp & 0x00ff00) << 8);
            display_color |= (color_temp & 0x0000ff);
            break;
        case LEDC_GRB://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            //格式一致，无需转换
            break;
        case LEDC_RBG://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 8);
            display_color |= ((color_temp & 0x00ff00) >> 8);
            display_color |= ((color_temp & 0x0000ff) << 16);
            break;
        case LEDC_GBR://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= (color_temp & 0xff0000);
            display_color |= ((color_temp & 0x00ff00) >> 8);
            display_color |= ((color_temp & 0x0000ff) << 8);
            break;
        case LEDC_BGR://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 16);
            display_color |= ((color_temp & 0x00ff00) << 8);
            display_color |= ((color_temp & 0x0000ff) << 8);
            break;
        case LEDC_BRG://实际测试ws2815输出为BRG格式，数据为小端格式，bit0-bit7对应B，bit8-bit15对应R，bit16-bit23对应G
            color_temp = display_color;
            display_color &= 0x000000;
            display_color |= ((color_temp & 0xff0000) >> 16);
            display_color |= (color_temp & 0x00ff00);
            display_color |= ((color_temp & 0x0000ff) << 16);
            break;
    }

    return display_color;
}

static void ws2815_rgb_color_out_32to24bit_code_fill(uint32_t buf_code_grb[],uint32_t buf_grb[])
{//小端格式
    unsigned int i = 0,i_add = 0;
    unsigned char j = 1;
    uint32_t but_temp = 0;

    uint32_t loop_cnt = LEDC_RGB_CODE_LEN / 3 * 4;

    for (i = 0; i < loop_cnt; i++) {
        switch (j) {
            case 0:
                buf_code_grb[i_add-1] = buf_grb[i];
                i_add--;
                j = j+2;
                break;
            case 1:
                buf_code_grb[i_add] = buf_grb[i];
                j++;
                break;
            case 2:
                but_temp = (buf_grb[i] & 0x000000ff);
                buf_code_grb[i_add-1] |= (but_temp << 24);
                buf_code_grb[i_add] = (buf_grb[i] >> 8);
                j++;
                break;
            case 3:
                but_temp = (buf_grb[i] & 0x0000ffff);
                buf_code_grb[i_add-1] |= (but_temp << 16);
                buf_code_grb[i_add] = (buf_grb[i] >> 16);
                j++;
                break;
            case 4:
                but_temp = (buf_grb[i] & 0x00ffffff);
                buf_code_grb[i_add-1] |= (but_temp << 8);
                buf_code_grb[i_add] = (buf_grb[i] >> 24);
                j = 0;
                break;
        }

        i_add++;
    }
}

/**
  * @brief  In order to keep DATA from application same as SPI data, We need to Transfer application data into DMA format(LSB for Hightest Byte first)
  *         For example:0x563412 means 0b 1010 0110 0011 0100 0001 0010 ----
  *                               ---->0b 0110 1010 0010 1100 0100 1000  ---> 0x6a2c48
  * @params[in] conver_color   The Address of target color buffer(conversion_color).
  *
  * @retval    None
  */
void bsp_data_to_dma_trans_format(uint32_t *conver_color)
{
    uint32_t color = *conver_color;

    uint32_t bit0 = (0x00111111 << 0) & color;
    uint32_t bit1 = (0x00111111 << 1) & color;
    uint32_t bit2 = (0x00111111 << 2) & color;
    uint32_t bit3 = (0x00111111 << 3) & color;

    *conver_color = (bit0 << 3) | (bit1 << 1) |  (bit2 >> 1) | (bit3 >> 3);

    color = *conver_color;

    uint32_t high_4bit = 0x00f0f0f0 & color;
    uint32_t low_4bit  = 0x000f0f0f & color;

    *conver_color = (high_4bit >> 4) | (low_4bit << 4);
}

void bsp_ws2815_ledc_init(uint8_t display_format)
{
    gpio_init_typedef gpio_init_structure;
    ledc_init_typedef ledc_init_struct;
    ledc_timing_init_typedef ledc_timing_init_struct;

    /* Config and enable clock */
    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_EN);
    clk_ledc_clk_set(CLK_LEDC_CLK_X24M_CLKDIV2);

    bsp_basetimer_init();

    /* LEDC Base Ctrl Init */
    ledc_init_struct.output_invert = LEDC_OUT_INV_DIS;
    ledc_init_struct.format = LEDC_IN_FORMAT_RGB;
    ledc_init_struct.idle_level = LEDC_IDLE_LEVEL_LOW;
    ledc_init_struct.valid_data_mode = LEDC_VALID_DATA_DIR;
    ledc_init_struct.byte_inv_number = LEDC_BYTE_INV_1B;
    ledc_init(&ledc_init_struct);

    /* LEDC Timing Init */
    ledc_timing_init_struct.baud = 24 - 1;      // 1 / (clk_12M / 24) = 2us
    ledc_timing_init_struct.delay = 24;         // (2us / 24) * 24 = 2us  interval between RGB and RGB
    ledc_timing_init_struct.code_0_high = 3;    // (2us / 24) * 3  = 250ns
    ledc_timing_init_struct.code_1_high = 11;   // (2us / 24) * 11 = 917ns
    ledc_timing_init_struct.reset_high = 10;    // 2us * 10  = 20us  interval between (LEDC last byte end) and (done pending)
    ledc_timing_init_struct.reset_low = 100;    // 2us * 100 = 200us
    ledc_timing_init(&ledc_timing_init_struct);

    /* GPIO Init */
    gpio_init_structure.gpio_pin = LEDC_OUTPUT_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_I_24MA | GPIOPB1_DRV_U_VBAT;

    /* Mapping Config */
    gpio_init(LEDC_OUTPUT_PORT, &gpio_init_structure);
    gpio_func_mapping_config(LEDC_OUTPUT_PORT, LEDC_OUTPUT_PIN, GPIO_CROSSBAR_OUT_LEDCDAT);

    /* Register interrupt, this interrupt source is not used by the module alone */
    /* Adjust the interrupt processing logic of related modules properly */
    ledc_pic_config(bsp_ledc_irq, 1, LEDC_FLAG_RST | LEDC_FLAG_DMA, ENABLE);

    /* Initialize procedure parameters */
    ledc_set_frame_length(LEDC_FRAME_LEN);
    ledc_set_loop_number(0);

    /* Startup module */
    ledc_cmd(ENABLE);

}

void bsp_ws2815_ledc_exec(uint8_t display_format)
{
    if (!bsp_ws2815_app_en) {
        return;
    }

    uint16_t i = 0;

    if (ledc_kick_state == 0) {

        switch (fill_state) {
            case 0:
                conversion_color = ws2815_rgb_color_format_conversion(SECOND_DISPLAY_COLOR,display_format);
                fill_state++;
                break;
            case 1:
                conversion_color = ws2815_rgb_color_format_conversion(THIRD_DISPLAY_COLOR,display_format);
                fill_state++;
                break;
            case 2:
                conversion_color = ws2815_rgb_color_format_conversion(FIRST_DISPLAY_COLOR,display_format);
                fill_state = 0;
                break;
        }

        for (i = 0; i < LEDC_FRAME_LEN; i++) {
            color_buf[i] = conversion_color;
        }
        #if DOUBLE_BUFFER_SWITCH
        if (switch_buf_state == 0) {
            ws2815_rgb_color_out_32to24bit_code_fill(&code_buf1[0],&color_buf[0]);
            switch_buf_state = 1;
            ledc_dma_kick((u32)&code_buf1[0], LEDC_RGB_CODE_LEN);
        }else if (switch_buf_state == 1) {
            ws2815_rgb_color_out_32to24bit_code_fill(&code_buf[0],&color_buf[0]);
            switch_buf_state = 0;
            ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);
        }

        #else

        ws2815_rgb_color_out_32to24bit_code_fill(&code_buf[0],&color_buf[0]);

        ledc_dma_kick((u32)&code_buf[0], LEDC_RGB_CODE_LEN);

        #endif
        ledc_kick_state = 1;
    }
}

#endif  //#if (BSP_LEDC_EN)


