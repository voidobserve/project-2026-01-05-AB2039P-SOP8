#ifndef _SPI1_SLAVE_H
#define _SPI1_SLAVE_H
#include "include.h"

/* SPI IO Definition */
#define SPI1_SLAVE_CLK_PORT                     GPIOB_REG
#define SPI1_SLAVE_CLK_PIN                      GPIO_PIN_0
#define SPI1_SLAVE_MISO_PORT                    GPIOB_REG
#define SPI1_SLAVE_MISO_PIN                     GPIO_PIN_1
#define SPI1_SLAVE_MOSI_PORT                    GPIOB_REG
#define SPI1_SLAVE_MOSI_PIN                     GPIO_PIN_2
#define SPI1_SLAVE_CS_PORT                      GPIOB_REG
#define SPI1_SLAVE_CS_PIN                       GPIO_PIN_4

void spi1_2wire_slave_init(u32 baud);
void spi1_2wire_slave_example(void);
void spi1_3wire_slave_init(u32 baud);
void spi1_3wire_slave_example(void);
#endif // _SPI1_FLASH_H
