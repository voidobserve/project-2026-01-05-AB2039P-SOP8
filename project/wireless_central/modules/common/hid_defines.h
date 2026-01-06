/*
 * @File name    : 
 * @Author       : Bluetrum IOT Team K.
 * @Date         : 2024-07-25
 * @Description  : 
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _HID_DEFINES_H
#define _HID_DEFINES_H


#define HID_REP_MAP_CONSUMER_SWAP(...) \
    0x05, 0x0C,         /* Usage Page (Consumer)                    */ \
    0x09, 0x01,         /* Usage (Consumer Control)                 */ \
    /* 0xA1, 0x01, */   /* Collection (Application)                 */ \
    __VA_ARGS__,        /* Report ID (...)                          */ \
    0x1A, 0x00, 0x00,   /* Usage Minimum (Unassigned)               */ \
    0x2A, 0x9C, 0x02,   /* Usage Maximum (AC Distribute Vertically) */ \
    0x16, 0x00, 0x00,   /* Logical minimum (0)                      */ \
    0x26, 0x9C, 0x02,   /* Logical maximum (668)                    */ \
    0x75, 0x10,         /* Report Size (16)                         */ \
    0x95, 0x01,         /* Report Count (1)                         */ \
    0x81, 0x00,         /* Input (Data,Array,Absolute,Bit Field)    */ \
    0xC0                /* End Collection                           */ \

#define HID_REP_MAP_KEYBOARD_SWAP(...) \
    0x05, 0x01,         /* USAGE_PAGE (Generic Desktop)                     */ \
    0x09, 0x06,         /* USAGE (Keyboard)                                 */ \
    /* 0xA1, 0x01, */   /* COLLECTION (Application)                         */ \
    __VA_ARGS__,        /* Report ID (...)                                  */ \
    0x05, 0x07,         /* USAGE_PAGE (Keyboard)                            */ \
    0x19, 0xE0,         /* USAGE_MINIMUM (Keyboard LeftControl)             */ \
    0x29, 0xE7,         /* USAGE_MAXIMUM (Keyboard Right GUI)               */ \
    0x15, 0x00,         /* LOGICAL_MINIMUM (0)                              */ \
    0x25, 0x01,         /* LOGICAL_MAXIMUM (1)                              */ \
    0x75, 0x01,         /* REPORT_SIZE (1)                                  */ \
    0x95, 0x08,         /* REPORT_COUNT (8)                                 */ \
    0x81, 0x02,         /* INPUT (Data,Var,Abs)                             */ \
    0x95, 0x01,         /* REPORT_COUNT (1)                                 */ \
    0x75, 0x08,         /* REPORT_SIZE (8)                                  */ \
    0x81, 0x03,         /* INPUT (Cnst,Var,Abs)                             */ \
    0x95, 0x05,         /* REPORT_COUNT (5)                                 */ \
    0x75, 0x01,         /* REPORT_SIZE (1)                                  */ \
    0x05, 0x08,         /* USAGE_PAGE (LEDs)                                */ \
    0x19, 0x01,         /* USAGE_MINIMUM (Num Lock)                         */ \
    0x29, 0x05,         /* USAGE_MAXIMUM (Kana)                             */ \
    0x91, 0x02,         /* OUTPUT (Data,Var,Abs)                            */ \
    0x95, 0x01,         /* REPORT_COUNT (1)                                 */ \
    0x75, 0x03,         /* REPORT_SIZE (3)                                  */ \
    0x91, 0x03,         /* OUTPUT (Cnst,Var,Abs)                            */ \
    0x95, 0x06,         /* REPORT_COUNT (6)                                 */ \
    0x75, 0x08,         /* REPORT_SIZE (8)                                  */ \
    0x15, 0x00,         /* LOGICAL_MINIMUM (0)                              */ \
    0x25, 0x65,         /* LOGICAL_MAXIMUM (101)                            */ \
    0x05, 0x07,         /* USAGE_PAGE (Keyboard)                            */ \
    0x19, 0x00,         /* USAGE_MINIMUM (Reserved (no event indicated))    */ \
    0x29, 0x65,         /* USAGE_MAXIMUM (Keyboard Application)             */ \
    0x81, 0x00,         /* INPUT (Data,Ary,Abs)                             */ \
    0xC0                /* END_COLLECTION                                   */

#define HID_REP_MAP_MOUSE_SWAP(...) \
    0x05, 0x01,         /* Usage Page (Generic Desktop)     */ \
    0x09, 0x02,         /* Usage (Mouse)                    */ \
    /* 0xA1, 0x01, */   /* COLLECTION (Application)         */ \
    __VA_ARGS__,        /* Report ID (...)                  */ \
    0x09, 0x01,         /* Usage (Pointer)                  */ \
    0xA1, 0x00,         /* Collection                       */ \
    0x05, 0x09,         /* Usage Page (Button)              */ \
    0x19, 0x01,         /* Usage Minimum (1)                */ \
    0x29, 0x05,         /* Usage Maximum (5)                */ \
    0x15, 0x00,         /* Logical Minimum (0)              */ \
    0x25, 0x01,         /* Logical Maximum (1)              */ \
    0x95, 0x05,         /* Report Count (5)                 */ \
    0x75, 0x01,         /* Report Size (1)                  */ \
    0x81, 0x02,         /* Input (Data, Variable, Absolute) */ \
    0x95, 0x01,         /* Report Count (1)                 */ \
    0x75, 0x03,         /* Report Size (3)                  */ \
    0x81, 0x01,         /* Input (Cnst, Ary, Abs)           */ \
    0x05, 0x01,         /* Usage Page (Generic Desktop)     */ \
    0x09, 0x30,         /* Usage (X)                        */ \
    0x09, 0x31,         /* Usage (Y)                        */ \
    0x16, 0x01, 0x80,   /* Logical Minimum (-32767)         */ \
    0x26, 0xFF, 0x7F,   /* Logical Maximum (32767)          */ \
    0x95, 0x02,         /* Report Count (2)                 */ \
    0x75, 0x10,         /* Report Size (16)                 */ \
    0x81, 0x06,         /* Input (Data, Variable, Relative) */ \
    0x09, 0x38,         /* Usage (Wheel)                    */ \
    0x15, 0x81,         /* Logical Minimum (-127)           */ \
    0x25, 0x7F,         /* Logical Maximum (127)            */ \
    0x95, 0x01,         /* Report Count (1)                 */ \
    0x75, 0x08,         /* Report Size (8)                  */ \
    0x81, 0x06,         /* Input (Data, Variable, Relative) */ \
    0xC0,               /* End Collection                   */ \
    0xC0                /* End Collection                   */

#define HID_REP_MAP_APP_SWAP(...) \
    0x06, 0xAB, 0xFF,   /* Usage Page (Vendor Defined)          */ \
    0x09, 0x00,         /* Usage (Undefined)                    */ \
    /* 0xA1, 0x01, */   /* COLLECTION (Application)             */ \
    __VA_ARGS__,        /* Report ID (...)                      */ \
    0x15, 0x00,         /* Logical Minimum (0)                  */ \
    0x25, 0xFF,         /* Logical Maximum (255)                */ \
    0x19, 0x00,         /* Usage Minimum (0)                    */ \
    0x29, 0xFF,         /* Usage Maximum (255)                  */ \
    0x95, 0x3f,         /* Report Count (63)                    */ \
    0x75, 0x08,         /* Report Size (8)                      */ \
    0x81, 0x02,         /* Input (Data, Variable, Absolute)     */ \
    0x19, 0x00,         /* Usage Minimum (0)                    */ \
    0x29, 0xFF,         /* Usage Maximum (255)                  */ \
    0x95, 0x3f,         /* Report Count (63)                    */ \
    0x75, 0x08,         /* Report Size (8)                      */ \
    0x91, 0x02,         /* Output (Data, Variable, Absolute)    */ \
    0xC0                /* End Collection                       */

#define HID_REP_MAP_DBG_SWAP(...) \
    0x06, 0xDB, 0xFF,   /* Usage Page (Vendor Defined)      */ \
    0x09, 0x00,         /* Usage (Undefined)                */ \
    /* 0xA1, 0x01, */   /* COLLECTION (Application)         */ \
    __VA_ARGS__,        /* Report ID (...)                  */ \
    0x15, 0x00,         /* Logical Minimum (0)              */ \
    0x25, 0xFF,         /* Logical Maximum (255)            */ \
    0x19, 0x00,         /* Usage Minimum (0)                */ \
    0x29, 0xFF,         /* Usage Maximum (255)              */ \
    0x95, 0x3f,         /* Report Count (64)                */ \
    0x75, 0x08,         /* Report Size (8)                  */ \
    0x81, 0x02,         /* Input (Data, Variable, Absolute) */ \
    0xC0                /* End Collection                   */


#define HID_REP_MAP_CONSUMER_NO_ID()            HID_REP_MAP_CONSUMER_SWAP(0xa1, 0x01)
#define HID_REP_MAP_CONSUMER(id)                HID_REP_MAP_CONSUMER_SWAP(0xa1, 0x01, 0x85, id)

#define HID_REP_MAP_KEYBOARD_NO_ID()            HID_REP_MAP_KEYBOARD_SWAP(0xa1, 0x01)
#define HID_REP_MAP_KEYBOARD(id)                HID_REP_MAP_KEYBOARD_SWAP(0xa1, 0x01, 0x85, id)

#define HID_REP_MAP_MOUSE_NO_ID()               HID_REP_MAP_MOUSE_SWAP(0xa1, 0x01)
#define HID_REP_MAP_MOUSE(id)                   HID_REP_MAP_MOUSE_SWAP(0xa1, 0x01, 0x85, id)

#define HID_REP_MAP_APP_NO_ID()                 HID_REP_MAP_APP_SWAP(0xa1, 0x01)
#define HID_REP_MAP_APP(id)                     HID_REP_MAP_APP_SWAP(0xa1, 0x01, 0x85, id)

#define HID_REP_MAP_DBG_NO_ID()                 HID_REP_MAP_DBG_SWAP(0xa1, 0x01)
#define HID_REP_MAP_DBG(id)                     HID_REP_MAP_DBG_SWAP(0xa1, 0x01, 0x85, id)


/**
 * @brief Determine by Peer/Local USB HID Report Map
 */
#define HID_REP_ID_CONSUMER                     0x01
#define HID_REP_ID_KEYBOARD                     0x02
#define HID_REP_ID_MOUSE                        0x03
#define HID_REP_ID_APP                          0xAB
#define HID_REP_ID_DGB                          0xDB


typedef struct PACKED {
    uint16_t        code;
} hid_rep_consumer_t;

typedef struct PACKED {
    struct {
        uint8_t     left_ctrl   : 1;
        uint8_t     left_shift  : 1;
        uint8_t     left_alt    : 1;
        uint8_t     left_gui    : 1;
        uint8_t     rigth_ctrl  : 1;
        uint8_t     rigth_shift : 1;
        uint8_t     rigth_alt   : 1;
        uint8_t     rigth_gui   : 1;
    };
    uint8_t         rsvd0;
    uint8_t         key[6];
} hid_rep_keyboard_t;

typedef struct PACKED {
    uint8_t         num_lock    : 1;
    uint8_t         caps_lock   : 1;
    uint8_t         scroll_lock : 1;
    uint8_t         compose     : 1;
    uint8_t         kana        : 1;
    uint8_t         reserved    : 3;
} hid_recv_led_t;

typedef struct PACKED {
    struct {
        uint8_t     btn_left    : 1;
        uint8_t     btn_right   : 1;
        uint8_t     btn_central : 1;
        uint8_t     btn_4       : 1;
        uint8_t     btn_5       : 1;
        uint8_t     reserved    : 3;
    };
    int16_t         x;
    int16_t         y;
    int8_t          wheel;
} hid_rep_mouse_t;

typedef struct PACKED {
    uint8_t         length;
    uint8_t         data[62];
} hid_rep_app_t;

typedef struct PACKED {
    uint8_t         data[63];
} hid_rep_dbg_t;

#endif // _HID_DEFINES_H
