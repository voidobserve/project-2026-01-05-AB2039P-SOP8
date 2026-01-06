#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if BSP_ADKEY_EN || BSP_IOKEY_EN
    #define BSP_KEY_EN              1
#endif

#if BSP_ADKEY_EN || BSP_VBAT_DETECT_EN
    #define BSP_SARADC_EN           1
#endif


#if !SYS_SLEEP_EN
    #undef SYS_SLEEP_TIME
    #define SYS_SLEEP_TIME          0
#endif

#if !AB_FOT_EN
    #undef AB_FOT_CHL_USB_EN
    #define AB_FOT_CHL_USB_EN       0
#endif

#if (FLASH_RESERVE_SIZE % 4096) != 0
    #error "FLASH_RESERVE_SIZE is not a multiple of 4096!"
#endif

#if AB_FOT_EN
    #if (FOT_PACK_START % 4096) != 0
        #error "FOT_PACK_START is not a multiple of 4096!"
    #endif
#endif

#if USB_EN && (BSP_UART_DEBUG_EN == GPIO_PB3)
#error "Could not use PB3 for PRINT when USB_EN!"
#endif

/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#if USB_EN
    #undef OS_THREAD_DRIVER_EN
    #define OS_THREAD_DRIVER_EN    1
#endif

#define OS_THREAD_SIZE_CAL(stack, msg, en)  (en ? (stack + 108 + msg * 112) : 0)
#define OS_HEAP_SIZE    768 +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_BT_STACK, 1, OS_THREAD_BT_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_DRIVER_STACK, 1, OS_THREAD_DRIVER_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_TIMER_STACK, 0, OS_THREAD_TIMER_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_MAIN_STACK, 0, 1)

/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/



/*****************************************************************************
 * Module    : UART打印功能配置
 *****************************************************************************/
#if !BSP_UART_DEBUG_EN
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif


#endif // __CONFIG_EXTRA_H__

