#include "include.h"
#include "spi1_flash.h"
#include "spi1_master.h"
#include "spi1_slave.h"
#include "spi1_example.h"
#include "bsp_uart_debug.h"

void spi1_example(void)
{
#if (SPI1_TEST_MODE == SPI1_3WIRE_TEST_FLASH)
    printf("--->spi example--->%s\n", TO_STRING(SPI1_3WIRE_TEST_FLASH));
    W25Qxx_init(50000);      // SPI clk select 24M speed.
    W25Qxx_example();
#elif (SPI1_TEST_MODE == SPI1_3WIRE_TEST_MASTER)
    printf("--->spi example--->%s\n", TO_STRING(SPI1_3WIRE_TEST_MASTER));
    spi1_3wire_master_init(50000);
    spi1_3wire_master_example();
#elif (SPI1_TEST_MODE == SPI1_3WIRE_TEST_SLAVE)
    printf("--->spi example--->%s\n", TO_STRING(SPI1_3WIRE_TEST_SLAVE));
    spi1_3wire_slave_init(50000);
    spi1_3wire_slave_example();
#elif (SPI1_TEST_MODE == SPI1_2WIRE_TEST_MASTER)
    printf("--->spi example--->%s\n", TO_STRING(SPI1_2WIRE_TEST_MASTER));
    spi1_2wire_master_init(50000);
    spi1_2wire_master_example();
#elif (SPI1_TEST_MODE == SPI1_2WIRE_TEST_SLAVE)
    printf("--->spi example--->%s\n", TO_STRING(SPI1_2WIRE_TEST_SLAVE));
    spi1_2wire_slave_init(50000);
    spi1_2wire_slave_example();
#endif
}
