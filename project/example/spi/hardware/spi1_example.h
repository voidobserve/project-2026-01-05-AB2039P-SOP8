#ifndef _SPI1_EXAMPLE_H
#define _SPI1_EXAMPLE_H
#include "driver_gpio.h"

//配置一
#define SPI1_2WIRE_TEST_MASTER          0
#define SPI1_2WIRE_TEST_SLAVE           1
#define SPI1_3WIRE_TEST_FLASH           2
#define SPI1_3WIRE_TEST_MASTER          3
#define SPI1_3WIRE_TEST_SLAVE           4
#define SPI1_TEST_MODE                  SPI1_3WIRE_TEST_FLASH

//配置二
#define SPI1_TEST_MASTER_DMA_MODE_EN    0  //1:dma mode.  0:buf mode
#define SPI1_TEST_SLAVE_DMA_MODE_EN     0  //1:dma mode.  0:buf mode

#define TO_STRING(X)                    #X

void spi1_example(void);
#endif // _SPI1_FLASH_H
