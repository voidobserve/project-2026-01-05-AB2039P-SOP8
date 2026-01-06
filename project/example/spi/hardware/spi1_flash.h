#ifndef _SPI1_FLASH_H
#define _SPI1_FLASH_H


/* 例程调试参数 */
#define W25Qxx_ID                           0x40140000  // Jedec唯一ID，通过读ID判断FLASH是否在线, 需要改成SPIFLASH对应的ID
#define W25Qxx_CHECK_ONLINE_ADDR            0
#define W25Qxx_CHECK_ONLINE_INFO            "SPI TEST"

/* SPI IO Definition */
#define W25Qxx_CLK_PORT                     GPIOB_REG
#define W25Qxx_CLK_PIN                      GPIO_PIN_0
#define W25Qxx_MISO_PORT                    GPIOB_REG
#define W25Qxx_MISO_PIN                     GPIO_PIN_1
#define W25Qxx_MOSI_PORT                    GPIOB_REG
#define W25Qxx_MOSI_PIN                     GPIO_PIN_2
#define W25Qxx_CS_PORT                      GPIOB_REG
#define W25Qxx_CS_PIN                       GPIO_PIN_4

/* 设备ID */
#define W25Q80                              0XEF13
#define W25Q16                              0XEF14
#define W25Q32                              0XEF15
#define W25Q64                              0XEF16
#define W25Q128                             0XEF17
#define W25Q256                             0XEF18

/* 指令表 */
#define W25Qxx_PAGE_PROGRAM                 0x02        // 页写
#define W25Qxx_READ_DATA                    0x03        // 读数据
#define W25Qxx_WRITE_ENABLE                 0x06        // 写使能
#define W25Qxx_WRITE_DISABLE                0x04        // 写失能
#define W25Qxx_SECTOR_ERASE                 0x20        // 扇区擦除(min erase unit 4096Byte)
#define W25Qxx_READ_STATUS_REG1             0x05        // 读SR1
#define W25Qxx_READ_STATUS_REG2             0x35
#define W25Qxx_READ_STATUS_REG3             0x15
#define W25Qxx_WRITE_STATUS_REG1            0x01
#define W25Qxx_WRITE_STATUS_REG2            0x31
#define W25Qxx_WRITE_STATUS_REG3            0x11
#define W25Qxx_FAST_READ_DATA               0x0B
#define W25Qxx_FAST_READ_DUAL               0x3B
#define W25Qxx_BLOCK_ERASE                  0xD8
#define W25Qxx_CHIP_ERASE                   0xC7
#define W25Qxx_POWER_DOWN                   0xB9
#define W25Qxx_RELEASE_POWER_DOWN           0xAB
#define W25Qxx_DEVICE_ID                    0xAB
#define W25Qxx_MANUFACT_DEVICE_ID           0x90
#define W25Qxx_JEDEC_DEVICE_ID              0x9F
#define W25Qxx_ENABLE_4BYTE_ADDR            0xB7
#define W25Qxx_EXIT_4BYTE_ADDR              0xE9

void W25Qxx_erase_sector(u32 addr);
void W25Qxx_erase_block(u32 addr);
void W25Qxx_erase_chip(void);
uint8_t W25Qxx_read_status_register(uint8_t idx);
void W25Qxx_read(u8 *buf, u32 addr, u32 len);
void W25Qxx_write_no_check(u8 *buf, u32 addr, u32 len);
void W25Qxx_write(u8 *buf, u32 addr, u32 len);
uint32_t W25Qxx_jedec_id_read(void);
uint16_t W25Qxx_manufact_id_read(void);

void W25Qxx_init(u32 baud);
void W25Qxx_example(void);

#endif // _SPI1_FLASH_H
