#ifndef _BSP_GSENSOR_H
#define _BSP_GSENSOR_H

#define BSP_GSENSOR_PORT            GPIOB_REG
#define BSP_GSENSOR_PIN             GPIO_PIN_5

#define bsp_gsensor_state_get()     gpio_read_bit(BSP_GSENSOR_PORT, BSP_GSENSOR_PIN)


void bsp_gsensor_init(void);
void bsp_gsensor_process(void);

#endif // _BSP_GSENSOR_H
