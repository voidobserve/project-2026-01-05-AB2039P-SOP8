/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef BLE_USB_DONGLE_CONFIG_H
#define BLE_USB_DONGLE_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_LE_FCC_EN                  0   //是否打开蓝牙FCC测试功能，需通过切换模式进入才能生效(func_cb.sta = FUNC_LE_FCC)
#define FUNC_LE_BQB_RF_EN               0   //是否打开蓝牙BQB测试功能，需通过切换模式进入才能生效(func_cb.sta = FUNC_LE_BQB_RF)
#define FUNC_IODM_EN                    0   //是否打开小牛测控功能, 使用单线串口通信,默认： VUSB, 115200
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_120M                //选择系统时钟
#define BUCK_MODE_EN                    0                       //是否BUCK MODE, 0:LDO MODE  1:BUCK MODE
#define PMU_CFG_VBAT                    PMU_CFG_VBAT_5V         //VBAT供电类型选择
#define WK0_10S_RESET                   0                       //WK pin 10秒复位功能
#define BSP_UART_DEBUG_EN               GPIO_PB2                //串口打印调试使能[Baud=1500000]
#define BSP_IOKEY_EN                    1                       //IO按鍵使能
#define BSP_SARADC_MIC_EN               0                       //MIC使能

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_128K                      //芯片内置flash空间大小
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
#define OS_THREAD_BT_EN                 1                                   //蓝牙线程使能
#define OS_THREAD_BT_STACK              1024                                //蓝牙线程堆栈大小
#define OS_THREAD_DRIVER_EN             1                                   //driver线程使能(有USB功能需使能)
#define OS_THREAD_DRIVER_STACK          1024                                //driver线程堆栈大小


/*****************************************************************************
 * Module    : BLE 功能配置
 *****************************************************************************/
#define LE_SM_EN                        1           //SM使能
#define LE_MTU_EXCHANGE_EN              1           //连接后主动发起MTU使能
#define LE_SM_SEND_SECURITY_REQ_TIME    500         //从机发起加密的延时，unit: ms
#define LE_SM_ALWAYS_ACCEPT_LTK_REQ_EN  0           //SM总是接受LTK请求使能
#define LE_DISCONN_RESTART_ADV_EN       1           //断连自动开广播使能
#define LE_RX_EXIT_SLEEP_EN             0           //RX到数据自动退出睡眠使能
#define LE_PHY_2M_EN                    1           //支持2M包使能
#define LE_PHY_UPDATE_EN                1           //连接后主动发起PHY使能
#define LE_FEATURE_EXCHANGE_EN          1           //连接后主动发起FEATURE使能
#define LE_LENGTH_REQ_EN                0           //连接后主动发起LENGTH REQUEST使能

#define LE_FCC_TEST_EN                  0           //上电直接进FCC，与 FUNC_LE_FCC_EN 共用串口配置
#define LE_FCC_TEST_HSUART_BAUD         1500000     //波特率
#define LE_FCC_TEST_HSUART_IO_SEL       GPIO_PB3    //IO口（默认单线串口）

#define LE_BQB_RF_EN                    0           //上电直接进BQB RF TEST， 与 FUNC_LE_BQB_RF_EN 共用串口配置
#define LE_BQB_RF_HSUART_BAUD           9600        //波特率
#define LE_BQB_RF_HSUART_RX_IO_SEL      GPIO_PB0    //IO口（默认双线串口）
#define LE_BQB_RF_HSUART_TX_IO_SEL      GPIO_PB1

/*****************************************************************************
 * Module    : 降噪算法配置
 *****************************************************************************/
#define NOTCH_IIR_EN                   0        //陷波器
#define NOTCH_IIR_PART_LEN1			   8        //8个样点处理一次

/*****************************************************************************
 * Module    : VOICE REMOTE DONGLE功能配置
 *****************************************************************************/
#define LE_VOICE_REMOTE_DONGLE_EN       0           //语音遥控器dongle使能，使用USB MIC通道传输
#define VC_RM_DG_ADPCM_DEC_EN           0           //ADPCM解码使能
#define VC_RM_DG_mSBC_DEC_EN            0           //mSBC解码使能
#define VC_RM_DG_SBC_DEC_EN             0           //SBC解码使能

/*****************************************************************************
 * Module    : usb device 配置
 *****************************************************************************/
#define USB_HID_EN                      1
#define USB_VENDOR_EN                   0
#define USB_MIC_EN                      0
#define USB_CDC_EN                      1                                                                   //USB CDC虚拟串口使能
#define USB_AUDIO_EN                    (USB_MIC_EN)

#define USB_EN                          (USB_HID_EN || USB_AUDIO_EN || USB_VENDOR_EN || USB_CDC_EN)         //USB功能使能
#define USB_ASSN_EN                     (USB_HID_EN && USB_CDC_EN)
#define USB_HID_REPORT_RATE_TEST        0                               //dongle侧USB播报率测试(WK0双击启动/单击停止)

/*****************************************************************************
 * Module    : 版本号管理
 *****************************************************************************/
#define SW_VERSION		                "V1.0.1"    //只能使用数字0-9,ota需要转码

#include "config_extra.h"

#endif
