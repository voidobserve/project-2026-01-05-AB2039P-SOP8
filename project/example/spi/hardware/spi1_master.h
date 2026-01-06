#ifndef _SPI1_MASTER_H
#define _SPI1_MASTER_H
#include "include.h"

/* SPI IO Definition */
#define SPI1_MASTER_CLK_PORT                     GPIOB_REG
#define SPI1_MASTER_CLK_PIN                      GPIO_PIN_0
#define SPI1_MASTER_MISO_PORT                    GPIOB_REG
#define SPI1_MASTER_MISO_PIN                     GPIO_PIN_1
#define SPI1_MASTER_MOSI_PORT                    GPIOB_REG
#define SPI1_MASTER_MOSI_PIN                     GPIO_PIN_2
#define SPI1_MASTER_CS_PORT                      GPIOB_REG
#define SPI1_MASTER_CS_PIN                       GPIO_PIN_4

void spi1_2wire_master_init(u32 baud);
void spi1_2wire_master_example(void);
void spi1_3wire_master_init(u32 baud);
void spi1_3wire_master_example(void);

#endif // _SPI1_FLASH_H
