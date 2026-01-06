/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef EXAMPLE_SARADC_SAMPLING_CONFIG_H
#define EXAMPLE_SARADC_SAMPLING_CONFIG_H

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

/*****************************************************************************
 * Module    : 降噪算法配置
 *****************************************************************************/
#define NOTCH_IIR_EN                   0      //陷波器
#define NOTCH_IIR_PART_LEN1			   128    //128个样点处理一次

/*****************************************************************************
 * Module    : 阈值跟踪闪烁算法配置
 *****************************************************************************/
#define THR_TRACK_FLASH_EN            1      //阈值跟踪闪烁算法
#define THR_TRACK_FLASH_PART_LEN	  128    //128个样点一帧
#define THR_TRACK_FLASH_SAMPLE_RATE	  8000   //8K采样率
#define THR_TRACK_FLASH_BUF_LEN	      6      //6帧输出一次鼓点判断值（(1/8000) * 128 * 6 = 96ms）
#define THR_TRACK_FLASH_DUMP_EN       0      //获取基础阈值（enr_nr_thr_base）

#endif // USER_CONFIG_H
