#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__


/*****************************************************************************
 * Module    : UART打印功能配置
 *****************************************************************************/
#if !UART_DEBUG_EN
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

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if (SYS_SLEEP_EN)
    #define SLEEP_SFR_SAVE          1
#else
    #define SLEEP_SFR_SAVE          0
#endif

#endif // __CONFIG_EXTRA_H__

