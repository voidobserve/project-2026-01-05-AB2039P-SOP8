#ifndef __WS2815_LEDC_H__
#define __WS2815_LEDC_H__

#define DMA_TICK_INTERVAL               (1000) //1s
#define DOUBLE_BUFFER_SWITCH            (0)
#define SAVE_MEMORY_ENABLE              (0)

#define FIRST_DISPLAY_COLOR       0X00ff0000
#define SECOND_DISPLAY_COLOR      0X0000ff00
#define THIRD_DISPLAY_COLOR       0X000000ff

#define LEDC_MORE_COLOR_DISPLAY_FORMAT         LEDC_RGB

//!!!!!!!!ram not enough, temporary modification!!!!!!!!
// #define LEDC_FRAME_LEN        768      //must be a multiple of 4, max value is 4096
#define LEDC_FRAME_LEN        40      //must be a multiple of 4, max value is 4096

#if (SAVE_MEMORY_ENABLE)
#define LEDC_RGB_CODE_LEN     (30)  //must be a multiple of 3
#else
#define LEDC_RGB_CODE_LEN     (unsigned int)(LEDC_FRAME_LEN - (unsigned int)(LEDC_FRAME_LEN/4))  //must be a multiple of 3
#endif
#define LEDC_WRGB_CODE_LEN    LEDC_FRAME_LEN   //ws2815 nonsupport 32bit out

#define LEDC_RGB         1
#define LEDC_GRB         2
#define LEDC_RBG         3
#define LEDC_GBR         4
#define LEDC_BGR         5
#define LEDC_BRG         6

void bsp_ws2815_ledc_init(uint8_t display_format);
void bsp_ws2815_ledc_exec(uint8_t display_format);
void bsp_ws2815_app_set(uint8_t enable);

#endif // __WS2815_LEDC_H__

