/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef WIRELESS_REMOTE_DONGLE_CONFIG_H
#define WIRELESS_REMOTE_DONGLE_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_WIRELESS_EN                1   //是否打开无线功能
#define FUNC_IODM_EN                    0   //是否打开小牛测控功能, 使用单线串口通信,默认： VUSB, 115200
#define FUNC_IDLE_EN                    1   //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_24M                 //选择系统时钟
#define BUCK_MODE_EN                    xcfg_cb.buck_mode_en    //0:LDO MODE  1:BUCK MODE
#define PMU_CFG_VBAT                    PMU_CFG_VBAT_4V2        //VBAT供电类型选择
#define WK0_10S_RESET                   1                       //WK pin 10秒复位功能
#define SYS_SLEEP_EN                    0                       //系统休眠使能
#define SYS_SLEEP_TIME                  0                       //自动休眠时间(秒), 0为不自动休眠
#define SYS_PWROFF_MODE                 PWROFF_MODE2            //软关机模式选择，模式1 VDDIO掉电，模式2 VDDIO不掉电
#define SYS_PWROFF_TIME                 0                       //自动关机时间(秒), 0为不自动关机
#define SYS_PWRUP_TIME                  0                       //长按开机时间(毫秒), 0为立即开机
#define BSP_UART_DEBUG_EN               GPIO_PB9                //串口打印调试使能[Baud=1500000]
#define BSP_ADKEY_EN                    0                       //AD按键使能
#define BSP_IOKEY_EN                    0                       //IO按鍵使能
#define BSP_VBAT_DETECT_EN              0                       //电量检测功能使能

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_256K                      //芯片内置flash空间大小
#define FLASH_RESERVE_SIZE              CM_SIZE                         //FLASH保留区域大小
#define FLASH_CODE_SIZE                 (FLASH_SIZE-FLASH_RESERVE_SIZE) //程序可使用的空间大小
#define FLASH_ERASE_4K                  1                               //是否支持4K擦除
#define FLASH_DUAL_READ                 0                               //是否支持2线模式
#define FLASH_QUAD_READ                 0                               //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                               //SPI FLASH提速

#define CM_SIZE                         0x2000                          //参数区大小，至少8k(需4K对齐)
#define CM_START                        (FLASH_SIZE - CM_SIZE)          //参数区默认放在flash最后面

/*****************************************************************************
 * Module    : FOTA功能配置
 *****************************************************************************/
#define AB_FOT_EN                       0                       //FOTA升级使能
#define LE_FOTA_EN                      AB_FOT_EN               //BLE FOTA升级使能
#define AB_FOT_CHL_USB_EN               1                       //FOTA升级USB通道使能

#if AB_FOT_EN
#if (FLASH_SIZE == FSIZE_256K)
#define FOT_PACK_START                  0x23000
#define FOT_PACK_SIZE                   0x19000
#elif (FLASH_SIZE == FSIZE_512K)
#define FOT_PACK_START                  0x4B000
#define FOT_PACK_SIZE                   0x32000
#else
#error "AB_FOTA: FOT_PACK_START not define, check FLASH_SIZE define or disable AB_FOT_EN\n"
#endif
#endif

/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#define OS_THREAD_MAIN_STACK            1024                                //main线程堆栈大小
#define OS_THREAD_TIMER_EN              0                                   //timer线程使能
#define OS_THREAD_TIMER_STACK           512                                 //timer线程堆栈大小
#define OS_THREAD_BT_EN                 0                                   //蓝牙线程使能
#define OS_THREAD_BT_STACK              1024                                //蓝牙线程堆栈大小
#define OS_THREAD_DRIVER_EN             1                                   //driver线程使能(有USB功能需使能)
#define OS_THREAD_DRIVER_STACK          1024                                //driver线程堆栈大小

/*****************************************************************************
 * Module    : 2.4G功能配置
 *****************************************************************************/
#define WIRELESS_PHY_FORCED             2               //Connection PHY forced (0: passive 1: 1MBPS 2: 2MBPS)
#define PRIVATE_FREQ_BANDS_SEL          0               //私有通信频段 (0: 2.4G 1: 2.3G: 2: 2.5G)
#define PRIVATE_ACCESS_CODE             0x878ED7B1      //私有接入码定义，主从需保持一致
#define WIRELESS_CONN_NB                2               //最大连接个数设置: 1 or 2

/*****************************************************************************
 * Module    : 音频配置
 *****************************************************************************/
#define AUDIO_MIC_PACKET_SIZE           (4+128)     //无线传输编码单包字节长度

/*****************************************************************************
 * Module    : usb device 配置
 *****************************************************************************/
#define USB_HID_EN                      1
#define USB_VENDOR_EN                   0
#define USB_MIC_EN                      1
#define USB_AUDIO_EN                    (USB_MIC_EN)
#define USB_EN                          (USB_HID_EN || USB_AUDIO_EN || USB_VENDOR_EN)

/*****************************************************************************
 * Module    : 版本号管理
 *****************************************************************************/
#define SW_VERSION		                "V1.0.1"    //只能使用数字0-9,ota需要转码

#include "config_extra.h"

#endif
