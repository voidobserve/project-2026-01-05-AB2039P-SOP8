#include "include.h"
#include "driver_com.h"
#include "driver_spi.h"
#include "driver_gpio.h"
#include "spi1_flash.h"
#include "spi1_example.h"

#if (SPI1_TEST_MODE == SPI1_3WIRE_TEST_FLASH)
AT(.user_buf.spi_sector_buf)
u8 W25Qxx_buffer[4096];         // 检查内数据是否需要擦除时的缓存区

// Dev is enabled when CS pin is low level.
static void spi1_cs_set(FUNCTIONAL_STATE dev_status)
{
    if (dev_status == DISABLE) {
        gpio_set_bits(W25Qxx_CS_PORT, W25Qxx_CS_PIN);
    } else {
        gpio_reset_bits(W25Qxx_CS_PORT, W25Qxx_CS_PIN);
    }
}

// SPI1接口获取1Byte数据
static uint8_t spi1_read_byte(void)
{
    spi_dir_sel(SPI1, SPI_DIR_RX);
    spi_send_data(SPI1, 0xff);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));
    return spi_receive_data(SPI1);
}

// SPI1接口发送1Byte数据
static void spi1_write_byte(uint8_t data)
{
    spi_dir_sel(SPI1, SPI_DIR_TX);
    spi_send_data(SPI1, data);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));
}

// SPIFlash写使能
static void W25Qxx_write_enable(void)
{
    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_WRITE_ENABLE);
    spi1_cs_set(DISABLE);
}

// SPIFlash等待空闲
static void W25Qxx_wait_busy(void)
{
    while (W25Qxx_read_status_register(1) & 0x01);
}

// SPIFlash 页写入
// buf: 待写入的数据buf
// addr: 目标地址
// len: 写入数据长度，不可超出页剩余字节数
static void W25Qxx_write_page(u8 *buf, u32 addr, u32 len)
{
    W25Qxx_write_enable();

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_PAGE_PROGRAM);

    spi1_write_byte(addr >> 16);
    spi1_write_byte(addr >> 8);
    spi1_write_byte(addr);

    spi_dir_sel(SPI1, SPI_DIR_TX);
    spi_set_dma_addr(SPI1, (u32)buf);
    spi_set_dma_cnt(SPI1, len);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING));                   //Wait pending

    spi1_cs_set(DISABLE);
    W25Qxx_wait_busy();
}

// SPIFlash读状态寄存器
// idx: W25Qxx有三个状态寄存器, 故idx范围(1~3)
uint8_t W25Qxx_read_status_register(uint8_t idx)
{
    uint8_t status_reg;
    u8 command;

    if (idx == 1) {
        command = W25Qxx_READ_STATUS_REG1;
    } else if (idx == 2) {
        command = W25Qxx_READ_STATUS_REG2;
    } else if (idx == 3) {
        command = W25Qxx_READ_STATUS_REG3;
    } else {
        return 0;
    }

    spi1_cs_set(ENABLE);
    spi1_write_byte(command);
    status_reg = spi1_read_byte();
    spi1_cs_set(DISABLE);

    return status_reg;
}

// SPIFlash读取设备唯一ID
uint32_t W25Qxx_jedec_id_read(void)
{
    uint32_t id = 0,i;
    spi1_cs_set(ENABLE);
    // spi1_delay();
    spi1_write_byte(W25Qxx_JEDEC_DEVICE_ID);
    for (i = 0; i < 4; i++) {
        id |= spi1_read_byte();
        id <<= 8;
    }
    spi1_cs_set(DISABLE);
    return id;
}

// SPIFlash读取设备类型ID
uint16_t W25Qxx_manufact_id_read(void)
{
    uint16_t tmp = 0;

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_MANUFACT_DEVICE_ID);
    spi1_write_byte(0x00);
    spi1_write_byte(0x00);
    spi1_write_byte(0x00);
    tmp |= spi1_read_byte() << 8;
    tmp |= spi1_read_byte();
    spi1_cs_set(DISABLE);

    return tmp;
}

// SPIFlash读取
void W25Qxx_read(u8 *buf, u32 addr, u32 len)
{
    u32 i;

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_READ_DATA);

    spi1_write_byte(addr >> 16);
    spi1_write_byte(addr >> 8);
    spi1_write_byte(addr);

    for (i = 0; i < len; i++) {
       buf[i] = spi1_read_byte();
    }

    spi1_cs_set(DISABLE);
}

// SPIFlash写操作，自动换页，但没有检验，使用前需保证对应区域全为0xff
void W25Qxx_write_no_check(u8 *buf, u32 addr, u32 len)
{
    u32 page_remain;

    page_remain = 256 - addr % 256;

    if (len <= page_remain) {
        page_remain = len;
    }

    while (1) {
        W25Qxx_write_page(buf, addr, page_remain);
        if (page_remain == len) {
            break;
        } else {
            buf += page_remain;
            addr += page_remain;
            len -= page_remain;
            page_remain = (len > 256)? 256: len;
        }
    }
}

// SPIFlash写操作，带擦除功能
void W25Qxx_write(u8 *buf, u32 addr, u32 len)
{
    u16 sector_idx;
    u16 sector_offset;
    u16 sector_remain;
    u8 *sector_buf;
    u16 i;

    sector_buf = W25Qxx_buffer;
    sector_idx = addr / 4096;
    sector_offset = addr % 4096;
    sector_remain = 4096 - sector_offset;

    if (len <= sector_remain) {
        sector_remain = len;
    }

    while (1) {
        W25Qxx_read(sector_buf, sector_idx * 4096, 4096);
        for (i = 0; i < sector_remain; i++) {
            if (sector_buf[sector_offset + 1] != 0xff) {
                break;
            }
        }

        if (i < sector_remain) {
            W25Qxx_erase_sector(addr);
            for (i = 0; i < sector_remain; i++) {
                sector_buf[sector_offset + i] = buf[i];
            }
            W25Qxx_write_no_check(sector_buf, sector_idx * 4096, 4096);
        } else {
            W25Qxx_write_no_check(buf, addr, sector_remain);
        }

        if (len == sector_remain) {
            break;
        } else {
            sector_idx++;
            sector_offset = 0;
            buf += sector_remain;
            addr += sector_remain;
            len -= sector_remain;
            sector_remain = (len > 4096)? 4096: len;
        }
    }
}

// 扇区擦除
// addr: 扇区内任意字节地址
void W25Qxx_erase_sector(u32 addr)
{
    W25Qxx_write_enable();

    W25Qxx_wait_busy();

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_SECTOR_ERASE);

    spi1_write_byte(addr >> 16);
    spi1_write_byte(addr >> 8);
    spi1_write_byte(addr);

    spi1_cs_set(DISABLE);

    W25Qxx_wait_busy();
}

// 块擦除
// addr: 块内任意地址
void W25Qxx_erase_block(u32 addr)
{
    W25Qxx_write_enable();

    W25Qxx_wait_busy();

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_BLOCK_ERASE);

    spi1_write_byte(addr >> 16);
    spi1_write_byte(addr >> 8);
    spi1_write_byte(addr);

    spi1_cs_set(DISABLE);

    W25Qxx_wait_busy();
}

// 全片擦除
void W25Qxx_erase_chip(void)
{
    W25Qxx_write_enable();

    W25Qxx_wait_busy();

    spi1_cs_set(ENABLE);
    spi1_write_byte(W25Qxx_CHIP_ERASE);
    spi1_cs_set(DISABLE);

    W25Qxx_wait_busy();
}

// 初始化W25Qxx例程
void W25Qxx_init(u32 baud)
{
    gpio_init_typedef gpio_init_structure;
    spi_init_typedef spi_init_structure;

    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);

    gpio_init_structure.gpio_pin = W25Qxx_CLK_PIN;              // CLK
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(W25Qxx_CLK_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = W25Qxx_MOSI_PIN;         // MOSI
    gpio_init(W25Qxx_MOSI_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = W25Qxx_MISO_PIN;         // MISO
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init(W25Qxx_MISO_PORT, &gpio_init_structure);

    gpio_init_structure.gpio_pin = W25Qxx_CS_PIN;           // CS
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init(W25Qxx_CS_PORT, &gpio_init_structure);

    spi1_cs_set(DISABLE);

    gpio_func_mapping_config(W25Qxx_CLK_PORT, W25Qxx_CLK_PIN, GPIO_CROSSBAR_OUT_SPI1CLK);
    gpio_func_mapping_config(W25Qxx_MISO_PORT, W25Qxx_MISO_PIN, GPIO_CROSSBAR_IN_SPI1DI0);
    gpio_func_mapping_config(W25Qxx_MOSI_PORT, W25Qxx_MOSI_PIN, GPIO_CROSSBAR_OUT_SPI1D0);

    spi_init_structure.baud = baud;
    spi_init_structure.bus_mode = SPI_3WIRE_MODE;
    spi_init_structure.role_mode = SPI_ROLE_MODE_MASTER;
    spi_init_structure.output_data_edge = SPI_OUTPUT_DATA_EDGE_FALLING;
    spi_init_structure.sample_clk_edge = SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF;
    spi_init(SPI1, &spi_init_structure);

    spi_cmd(SPI1, ENABLE);
}

// 执行W25Qxx例程
void W25Qxx_example(void)
{
    u8 has_checked_online = 0;
    u8 read_buf[20];
    u8 check_info[] = W25Qxx_CHECK_ONLINE_INFO;
    u8 check_info_len = sizeof(W25Qxx_CHECK_ONLINE_INFO);
    u16 check_addr = W25Qxx_CHECK_ONLINE_ADDR;
    u32 flash_id;
    printf("W25Qxx_example\n");

    while (1) {
        if (!has_checked_online) {
            flash_id = W25Qxx_jedec_id_read();
            printf("Flash ID: 0x%08X\n", flash_id);
            if (flash_id == W25Qxx_ID) {
                printf("Manufactor ID: %X\n", W25Qxx_manufact_id_read());

                W25Qxx_read(read_buf, check_addr, check_info_len);
                if (memcmp(read_buf, check_info, check_info_len) == 0) {
                    printf("flash not first connect: %s\n", read_buf);
                } else {
                    W25Qxx_write(check_info, check_addr, check_info_len);
                    W25Qxx_read(read_buf, check_addr, check_info_len);
                    printf("flash first connect: %s\n", read_buf);
                }
                has_checked_online = 1;
            } else {
                printf("Flash ID Error~\n");
            }
        } else {
            printf("system is running\n");
        }

        delay_ms(1000);
    }
}

#endif  //SPI1_3WIRE_TEST_FLASH
