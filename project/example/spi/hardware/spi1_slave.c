#include "include.h"
#include "driver_com.h"
#include "driver_spi.h"
#include "driver_gpio.h"
#include "spi1_slave.h"
#include "spi1_example.h"

#if (SPI1_TEST_MODE == SPI1_2WIRE_TEST_SLAVE)
// SPI1接口设置IO口输入输出方向
void spi1_2wire_slave_set_dir(GPIO_DIR_TYPEDEF dir)
{
    if (dir == GPIO_DIR_OUTPUT) {
        gpio_set_dir(SPI1_SLAVE_MOSI_PORT, SPI1_SLAVE_MOSI_PIN, GPIO_DIR_OUTPUT);
        spi_dir_sel(SPI1, SPI_DIR_TX);
    } else {
        gpio_set_dir(SPI1_SLAVE_MOSI_PORT, SPI1_SLAVE_MOSI_PIN, GPIO_DIR_INPUT);
        spi_dir_sel(SPI1, SPI_DIR_RX);
    }
}

// SPI1接口获取1Byte数据
uint8_t spi1_2wire_slave_read_byte(void)
{
    spi_send_data(SPI1, 0xff);  //kick

    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting rx pending
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);

    return spi_receive_data(SPI1);
}

// SPI1接口发送1Byte数据
void spi1_2wire_slave_write_byte(uint8_t data)
{
    spi_send_data(SPI1, data);  //kick and send data

    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
}

// 初始化spi1_2wire_slave例程
void spi1_2wire_slave_init(u32 baud)
{
    gpio_init_typedef gpio_init_structure;
    spi_init_typedef spi_init_structure;

    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);

    gpio_init_structure.gpio_pin = SPI1_SLAVE_CLK_PIN;           // CLK
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(SPI1_SLAVE_CLK_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pin = SPI1_SLAVE_MOSI_PIN;         // MOSI
    gpio_init(SPI1_SLAVE_MOSI_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = SPI1_SLAVE_CS_PIN;           // CS
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init(SPI1_SLAVE_CS_PORT, &gpio_init_structure);

    gpio_func_mapping_config(SPI1_SLAVE_CLK_PORT, SPI1_SLAVE_CLK_PIN, GPIO_CROSSBAR_IN_SPI1CLK);
    gpio_func_mapping_config(SPI1_SLAVE_MOSI_PORT, SPI1_SLAVE_MOSI_PIN, GPIO_CROSSBAR_IN_SPI1DI0);
    gpio_func_mapping_config(SPI1_SLAVE_MOSI_PORT, SPI1_SLAVE_MOSI_PIN, GPIO_CROSSBAR_OUT_SPI1D0);

    spi_init_structure.baud = baud;
    spi_init_structure.bus_mode = SPI_2WIRE_MODE;
    spi_init_structure.role_mode = SPI_ROLE_MODE_SLAVE;
    spi_init_structure.output_data_edge = SPI_OUTPUT_DATA_EDGE_FALLING;
    spi_init_structure.sample_clk_edge = SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF;
    spi_init_structure.clkids = SPI_CLOCK_IDLE_HIGH;
    spi_init(SPI1, &spi_init_structure);

    spi_cmd(SPI1, ENABLE);
}
// 执行spi1_2wire_slave例程
void spi1_2wire_slave_example(void)
{
    uint8_t write_data[3] = {0x04, 0x05, 0x06};
    uint8_t read_data[3];
    while(1) {
        /*CS select*/
        while (RESET == (gpio_read_bit(SPI1_SLAVE_CS_PORT, SPI1_SLAVE_CS_PIN))) {
#if (SPI1_TEST_SLAVE_DMA_MODE_EN == 0)
                spi1_2wire_slave_set_dir(GPIO_DIR_INPUT);
                read_data[0] = spi1_2wire_slave_read_byte();
                read_data[1] = spi1_2wire_slave_read_byte();
                read_data[2] = spi1_2wire_slave_read_byte();

                spi1_2wire_slave_set_dir(GPIO_DIR_OUTPUT);
                spi1_2wire_slave_write_byte(write_data[0]);
                spi1_2wire_slave_write_byte(write_data[1]);
                spi1_2wire_slave_write_byte(write_data[2]);
#else                
                spi1_2wire_slave_set_dir(GPIO_DIR_INPUT);
                spi_set_dma_addr(SPI1, (uint32_t)&read_data);
                spi_set_dma_cnt(SPI1, sizeof(read_data));
                while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
                spi_clear_flag(SPI1, SPI_FLAG_PENDING);
                
                spi1_2wire_slave_set_dir(GPIO_DIR_OUTPUT);
                spi_set_dma_addr(SPI1, (uint32_t)&write_data);
                spi_set_dma_cnt(SPI1, sizeof(write_data));
                while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
                spi_clear_flag(SPI1, SPI_FLAG_PENDING);
#endif  //(SPI1_TEST_SLAVE_DMA_MODE_EN == 0)                
        
                printf("S:0x%x, 0x%x, 0x%x\n", read_data[0], read_data[1], read_data[2]);
            }
        }
}

#elif (SPI1_TEST_MODE == SPI1_3WIRE_TEST_SLAVE)

// SPI1接口获取1Byte数据
uint8_t spi1_3wire_slave_read_byte(void)
{
    spi_send_data(SPI1, 0xff);  //kick

    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting rx pending
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);

    return spi_receive_data(SPI1);
}

// SPI1接口发送1Byte数据
void spi1_3wire_slave_write_byte(uint8_t data)
{
    spi_send_data(SPI1, data);  //kick and send data

    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
}

// 初始化spi1_3wire_slave例程
void spi1_3wire_slave_init(u32 baud)
{
    gpio_init_typedef gpio_init_structure;
    spi_init_typedef spi_init_structure;

    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);

    gpio_init_structure.gpio_pin = SPI1_SLAVE_CLK_PIN;           // CLK
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(SPI1_SLAVE_CLK_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pin = SPI1_SLAVE_MOSI_PIN;         // MOSI
    gpio_init(SPI1_SLAVE_MOSI_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_pin = SPI1_SLAVE_MISO_PIN;         // MISO
    gpio_init(SPI1_SLAVE_MISO_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = SPI1_SLAVE_CS_PIN;           // CS
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init(SPI1_SLAVE_CS_PORT, &gpio_init_structure);

    gpio_func_mapping_config(SPI1_SLAVE_CLK_PORT, SPI1_SLAVE_CLK_PIN, GPIO_CROSSBAR_IN_SPI1CLK);
    gpio_func_mapping_config(SPI1_SLAVE_MOSI_PORT, SPI1_SLAVE_MOSI_PIN, GPIO_CROSSBAR_IN_SPI1DI0);
    gpio_func_mapping_config(SPI1_SLAVE_MISO_PORT, SPI1_SLAVE_MISO_PIN, GPIO_CROSSBAR_OUT_SPI1D0);

    spi_init_structure.baud = baud;
    spi_init_structure.bus_mode = SPI_3WIRE_MODE;
    spi_init_structure.role_mode = SPI_ROLE_MODE_SLAVE;
    spi_init_structure.output_data_edge = SPI_OUTPUT_DATA_EDGE_FALLING;
    spi_init_structure.sample_clk_edge = SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF;
    spi_init_structure.clkids = SPI_CLOCK_IDLE_HIGH;
    spi_init(SPI1, &spi_init_structure);

    spi_cmd(SPI1, ENABLE);
}

// 执行spi1_3wire_slave例程
void spi1_3wire_slave_example(void)
{
    uint8_t write_data[3] = {0x04, 0x05, 0x06};
    uint8_t read_data[3];
    while(1) {
        /*CS select*/
        while (RESET == (gpio_read_bit(SPI1_SLAVE_CS_PORT, SPI1_SLAVE_CS_PIN))) {
#if (SPI1_TEST_SLAVE_DMA_MODE_EN == 0)
                read_data[0] = spi1_3wire_slave_read_byte();
                read_data[1] = spi1_3wire_slave_read_byte();
                read_data[2] = spi1_3wire_slave_read_byte();

                spi1_3wire_slave_write_byte(write_data[0]);
                spi1_3wire_slave_write_byte(write_data[1]);
                spi1_3wire_slave_write_byte(write_data[2]);
#else
                spi_dir_sel(SPI1, SPI_DIR_RX);
                spi_set_dma_addr(SPI1, (uint32_t)&read_data);
                spi_set_dma_cnt(SPI1, sizeof(read_data));
                while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
                spi_clear_flag(SPI1, SPI_FLAG_PENDING);

                spi_dir_sel(SPI1, SPI_DIR_TX);
                spi_set_dma_addr(SPI1, (uint32_t)&write_data);
                spi_set_dma_cnt(SPI1, sizeof(write_data));
                while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));  // waiting tx pending
                spi_clear_flag(SPI1, SPI_FLAG_PENDING);
#endif  //(SPI1_TEST_SLAVE_DMA_MODE_EN == 0)
        
                printf("S:0x%x, 0x%x, 0x%x\n", read_data[0], read_data[1], read_data[2]);
            }
        }
}

#endif  //SPI1_TEST_MODE  SLAVE
