#include "include.h"
#include "driver_ledc.h"
#include "driver_gpio.h"
#include "ledc.h"

/****************************************************************************************************************************************************/
/**********************************************************！！READ ME！！***************************************************************************/
/***************************************************************************************************************************************************/

//     byte0     byte1     byte2     byte3     byte4     byte5     byte6     byte7         ......
//RGB:   R        G          B [delay] R         G         B [delay] R         G           ......
//WRGB:  W        R          G         B [delay] W         R         G         B   [delay] ......

//-------------------------------------------------|dma pending|-----------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------|done pending|----------------------------
//------------------------------------------------------------------------------------------------------------------------|reset pending（finish）|-
//
//ledc_set_loop_number(loop)、LEDC_FRAME_LENGTH(num)： 
//             （常规）以RGB为例，当loop等于 0 或 1时，ledc输出num种灯效，每3byte表示一种灯效，需要3*num byte，可以点亮num个灯。
//             （特殊）以RGB为例，当loop大于 1 时，ledc输出loop种灯效，每3byte表示一种灯的等效，需要3*loop byte，每种灯效重复num次，可以点亮loop * num个灯。
//数据反转类型：
//      LEDC_BYTE_INV_1B:  0x44332211 ->  0x22cc4488
//      LEDC_BYTE_INV_2B:  0x44332211 ->  0xcc228844
//      LEDC_BYTE_INV_4B:  0x44332211 ->  0x8844cc22
//
/***************************************************************************************************************************************************/
/***************************************************************************************************************************************************/
/***************************************************************************************************************************************************/
#define LEDC_RGB_EN                  1            //0:WRGB; 1:RGB
#define LEDC_LOOP_NUM                0            //特殊循环，阅读上述 ！！READ ME！！
#define LEDC_FRAME_LENGTH            (3*4)        //4的倍数，当loop == 0时，该宏表示灯数. RGB时，unit:3byte; WRGB时， unit:4byte.

#if LEDC_RGB_EN
#define LEDC_DMA_CNT                 (LEDC_FRAME_LENGTH - (LEDC_FRAME_LENGTH / 4))  //unit: 4 byte
#define LEDC_FRAME_BUF_LEN           (LEDC_FRAME_LENGTH * 3)    //unit: 1 byte
#else
#define LEDC_DMA_CNT                 LEDC_FRAME_LENGTH
#define LEDC_FRAME_BUF_LEN           (LEDC_FRAME_LENGTH * 4)
#endif  //LEDC_RGB_EN

#define LEDC_DMA_KICK_NUM            3            //一帧KICK NUM 次DMA，可以省ram，注意！LEDC_DMA_KICK_NUM 能被 LEDC_FRAME_BUF_LEN整除
#define LEDC_DMA_BUF_LEN             (LEDC_DMA_CNT * 4 / LEDC_DMA_KICK_NUM)  // unit: 1 byte


#define LEDC_REFRESH_TIME            1000         //ms
     
#define LEDC_OUT_PORT                GPIOB_REG
#define LEDC_OUT_PIN                 GPIO_PIN_5
     
static bool ledc_finish_flag = 0;
static uint8_t effect_num = 0;  //RGB  0:全部红灯; 1:全部绿灯; 2:全部蓝; 3:全部熄灭
static uint8_t ledc_dma_offset = 0;
static uint8_t ledc_dma_buf[LEDC_DMA_BUF_LEN];  

int8_t ledc_fill_dma_buffer_test( uint8_t effect_num);

//RGB灯效0, 一个灯 3 byte
static uint8_t rgb_lamp_effect_0[LEDC_FRAME_BUF_LEN] = {
//    R      G      B
    0xff,  0x00,  0x00,  //第一个灯
    0xff,  0x00,  0x00,  //第二个灯
    0xff,  0x00,  0x00,
    0xff,  0x00,  0x00,  
    0xff,  0x00,  0x00,  
    0xff,  0x00,  0x00,
    0xff,  0x00,  0x00,  
    0xff,  0x00,  0x00,  
    0xff,  0x00,  0x00,
    0xff,  0x00,  0x00,
    0xff,  0x00,  0x00,
    0xff,  0x00,  0x00, //第 LEDC_FRAME_LENGTH 个灯
};

//RGB灯效1
static uint8_t rgb_lamp_effect_1[LEDC_FRAME_BUF_LEN] = {
//    R      G      B
    0x00,  0xff,  0x00,  //第一个灯
    0x00,  0xff,  0x00,  //第二个灯
    0x00,  0xff,  0x00,
    0x00,  0xff,  0x00,  
    0x00,  0xff,  0x00,  
    0x00,  0xff,  0x00,
    0x00,  0xff,  0x00,  
    0x00,  0xff,  0x00,  
    0x00,  0xff,  0x00,
    0x00,  0xff,  0x00,
    0x00,  0xff,  0x00,
    0x00,  0xff,  0x00, //第 LEDC_FRAME_LENGTH 个灯
};

//RGB灯效2
static uint8_t rgb_lamp_effect_2[LEDC_FRAME_BUF_LEN] = {
//    R      G      B
    0x00,  0x00,  0xff,  //第一个灯
    0x00,  0x00,  0xff,  //第二个灯
    0x00,  0x00,  0xff,
    0x00,  0x00,  0xff,  
    0x00,  0x00,  0xff,  
    0x00,  0x00,  0xff,
    0x00,  0x00,  0xff,  
    0x00,  0x00,  0xff,  
    0x00,  0x00,  0xff,
    0x00,  0x00,  0xff,
    0x00,  0x00,  0xff,
    0x00,  0x00,  0xff, //第 LEDC_FRAME_LENGTH 个灯
};

//RGB灯效3
static uint8_t rgb_lamp_effect_3[LEDC_FRAME_BUF_LEN] = {
//    R      G      B
    0x00,  0x00,  0x00,  //第一个灯
    0x00,  0x00,  0x00,  //第二个灯
    0x00,  0x00,  0x00,
    0x00,  0x00,  0x00,  
    0x00,  0x00,  0x00,  
    0x00,  0x00,  0x00,
    0x00,  0x00,  0x00,  
    0x00,  0x00,  0x00,  
    0x00,  0x00,  0x00,
    0x00,  0x00,  0x00,
    0x00,  0x00,  0x00,
    0x00,  0x00,  0x00, //第 LEDC_FRAME_LENGTH 个灯
};

////注意：bsp_ledc_isr是高优先级中断服务函数，不能在该函数中加太多处理代码，以免影响其他代码的执行！！
AT(.com_text.isr) FIQ
void ledc_irq_handler(void)
{
    if (ledc_get_flag(LEDC_FLAG_DONE)) {
        ledc_clear_flag(LEDC_FLAG_DONE);
    }

    if (ledc_get_flag(LEDC_FLAG_RST)) {
        ledc_clear_flag(LEDC_FLAG_RST);
        ledc_finish_flag = 1;
    }

     if (ledc_get_flag(LEDC_FLAG_DMA)) {
        ledc_clear_flag(LEDC_FLAG_DMA);
        int8_t fill_res =  ledc_fill_dma_buffer_test(effect_num);
        if (fill_res != -1) {
            ledc_dma_kick((uint32_t)ledc_dma_buf, LEDC_DMA_CNT/LEDC_DMA_KICK_NUM);
        }
    }
}

AT(.com_text.ledc)
int8_t ledc_fill_dma_buffer_test( uint8_t effect_num)
{
    uint8_t vaild = 1;

    if (ledc_dma_offset == LEDC_DMA_KICK_NUM) {
        ledc_dma_offset = 0;
        return -1;
    }

    switch (effect_num) {
        case 0:
                memcpy(ledc_dma_buf, &rgb_lamp_effect_0[ledc_dma_offset * LEDC_DMA_BUF_LEN], LEDC_DMA_BUF_LEN);
            break;

        case 1:
                memcpy(ledc_dma_buf, &rgb_lamp_effect_1[ledc_dma_offset * LEDC_DMA_BUF_LEN], LEDC_DMA_BUF_LEN);
            break;

        case 2:
                memcpy(ledc_dma_buf, &rgb_lamp_effect_2[ledc_dma_offset * LEDC_DMA_BUF_LEN], LEDC_DMA_BUF_LEN);
            break;

        case 3:
                memcpy(ledc_dma_buf, &rgb_lamp_effect_3[ledc_dma_offset * LEDC_DMA_BUF_LEN], LEDC_DMA_BUF_LEN);
            break;

        default:
            vaild = 0;
            break;
    }
    
    if (vaild) {
        ledc_dma_offset++;
    }

    return ledc_dma_offset;
}

void ledc_example_init(void)
{
    gpio_init_typedef gpio_init_struct;
    ledc_init_typedef ledc_init_struct;
    ledc_timing_init_typedef ledc_timing_init_struct;

    /* Config and enable clock */
    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_EN);
    clk_ledc_clk_set(CLK_LEDC_CLK_X24M_CLKDIV2);

    /* GPIO Init */
    gpio_init_struct.gpio_pin = LEDC_OUT_PIN;
    gpio_init_struct.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen = GPIO_FEN_PER;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_struct.gpio_drv = GPIO_DRV_I_6MA;
    gpio_init(LEDC_OUT_PORT, &gpio_init_struct);

    /* Config GPIO Mapping */
    gpio_func_mapping_clear(LEDC_OUT_PORT, LEDC_OUT_PIN);
    gpio_func_mapping_config(LEDC_OUT_PORT, LEDC_OUT_PIN, GPIO_CROSSBAR_OUT_LEDCDAT);

    /* LEDC Base Ctrl Init */
    ledc_init_struct.output_invert = LEDC_OUT_INV_DIS;
#if LEDC_RGB_EN
    ledc_init_struct.format = LEDC_IN_FORMAT_RGB;
#else
    ledc_init_struct.format = LEDC_IN_FORMAT_WRGB;
#endif //LEDC_RGB_EN
    ledc_init_struct.idle_level = LEDC_IDLE_LEVEL_LOW;
    ledc_init_struct.valid_data_mode = LEDC_VALID_DATA_DIR;
    ledc_init_struct.byte_inv_number = LEDC_BYTE_INV_1B;  //WS2815
    ledc_init(&ledc_init_struct);

    /* LEDC Timing Init */
    ledc_timing_init_struct.baud = 24 - 1;      // 1 / (clk_12M / 24) = 2us
    ledc_timing_init_struct.delay = 24;         // (2us / 24) * 24 = 2us  interval between RGB and RGB
    ledc_timing_init_struct.code_0_high = 3;    // (2us / 24) * 3  = 250ns
    ledc_timing_init_struct.code_1_high = 11;   // (2us / 24) * 11 = 917ns
    ledc_timing_init_struct.reset_high = 10;    // 2us * 10  = 20us  interval between (LEDC last byte end) and (done pending)
    ledc_timing_init_struct.reset_low = 100;    // 2us * 100 = 200us
    ledc_timing_init(&ledc_timing_init_struct);

    /* Register interrupt, this interrupt source is not used by the module alone */
    ledc_pic_config(ledc_irq_handler, 1, LEDC_FLAG_RST|LEDC_FLAG_DMA|LEDC_FLAG_DONE, ENABLE);

    /* Initialize procedure parameters */
    ledc_set_frame_length(LEDC_FRAME_LENGTH);
    ledc_set_loop_number(LEDC_LOOP_NUM);

    /* Startup module, make sure to enable the module before starting DMA */
    ledc_cmd(ENABLE);
}

void ledc_example_loop(void)
{
    int8_t fill_res;
    static uint32_t tick;
    tick = tick_get();

    fill_res = ledc_fill_dma_buffer_test(effect_num);
    if (fill_res != -1) {
        ledc_dma_kick((uint32_t)ledc_dma_buf, LEDC_DMA_CNT/LEDC_DMA_KICK_NUM);  //first kick
        ledc_kick();  //first kick
    }

    while(1){
        WDT_CLR();
        if (tick_check_expire(tick, LEDC_REFRESH_TIME) && (ledc_finish_flag == 1)) {
            tick = tick_get();
            ledc_finish_flag = 0;

            effect_num = (effect_num == 3) ? 0 : (effect_num+1);
            printf("effect_num:%d\n", effect_num);
            fill_res =  ledc_fill_dma_buffer_test(effect_num);
            if (fill_res != -1) {
                ledc_dma_kick((uint32_t)ledc_dma_buf, LEDC_DMA_CNT/LEDC_DMA_KICK_NUM);
                ledc_kick();
            }
        }
    }
}
