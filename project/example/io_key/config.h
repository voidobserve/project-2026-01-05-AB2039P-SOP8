/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef EXAMPLE_IO_KEY_CONFIG_H
#define EXAMPLE_IO_KEY_CONFIG_H

#include "config_define.h"


/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_128K                      //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              0x2000                          //程序保留区空间大小（CM_SIZE）
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序使用空间大小
#define FLASH_ERASE_4K                  1                               //是否支持4K擦除
#define FLASH_DUAL_READ                 0                               //是否支持2线模式
#define FLASH_QUAD_READ                 0                               //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                               //SPI FLASH提速。

#endif // USER_CONFIG_H
