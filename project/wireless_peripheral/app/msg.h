#ifndef _MSG_H
#define _MSG_H

//Event Message, 范围：0x700 ~ 0x7ff
enum{
    MSG_SYS_1S = 0x700,
    MSG_SYS_500MS,

    MSG_SYS_PC_INSERT,
    MSG_SYS_PC_REMOVE,

    MSG_SYS_MAX = 0x7ff,
};

// Thread Driver Message
enum {
    THREAD_DRIVER_MSG_HID_IN,
    THREAD_DRIVER_MSG_VENDOR_IN,
    THREAD_DRIVER_MSG_USB_IND,
    THREAD_DRIVER_MSG_UAC_MIC_CTRL,
    THREAD_DRIVER_MSG_UAC_DEC,
};

#endif 