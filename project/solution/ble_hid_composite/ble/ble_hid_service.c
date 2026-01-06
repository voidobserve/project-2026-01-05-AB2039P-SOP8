#include "include.h"
#include "ble_hid_service.h"
#include "app_hid_tiktok.h"
#include "app_hid_selfie.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif


static att_service_handler_t    ble_service;
static uint16_t                 ble_report_digitizer_client_config;
static uint16_t                 ble_report_mouse_client_config;
static uint16_t                 ble_report_consumer_client_config;
static uint16_t                 ble_report_keyboard_client_config;
static uint16_t                 ble_connect_handle;

static ble_hid_tasks_typedef    ble_hid_tasks;

const u8 report_map_android[]={
#if LE_HID_CONSUMER
#if (BLE_HID_SELFIE_MODE || BLE_HID_TIKTOK_MODE)
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x09, 0x30,        //   Usage (Power)
    0x09, 0x40,        //   Usage (Menu)
    0x09, 0x82,        //   Usage (Mode Step)
    0x09, 0xA0,        //   Usage (VCR Plus)
    0x09, 0xB5,        //   Usage (Scan Next Track)
    0x09, 0xB6,        //   Usage (Scan Previous Track)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x09, 0xE2,        //   Usage (Mute)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x0A, 0x83, 0x01,  //   Usage (AL Consumer Control Configuration)
    0x0A, 0x96, 0x01,  //   Usage (AL Internet Browser)
    0x0A, 0x23, 0x02,  //   Usage (AC Home)
    0x0A, 0x24, 0x02,  //   Usage (AC Back)
    0x0A, 0x2D, 0x02,  //   Usage (AC Zoom In)
    0x0A, 0x2E, 0x02,  //   Usage (AC Zoom Out)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x29, 0x01,        //   Usage Maximum (Consumer Control)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
#else
	0x05, 0x0C, 		//  Usage Page (Consumer)
	0x09, 0x01, 		//  Usage (Consumer Control)
	0xA1, 0x01, 		//  Collection (Application)
	0x85, 0x01, 		//	Report Id (1)
	0x1A, 0x00, 0x00,	//	Usage Minimum (Unassigned)
	0x2A, 0x9C, 0x02,	//	Usage Maximum (AC Distribute Vertically)
	0x16, 0x00, 0x00,	//	Logical minimum (0)
	0x26, 0x9C, 0x02,	//	Logical maximum (668)
	0x75, 0x10, 		//	Report Size (16)
	0x95, 0x01, 		//	Report Count (1)
	0x81, 0x00, 		//	Input (Data,Array,Absolute,Bit Field)
	0xC0,				//  End Collection
#endif //(BLE_HID_SELFIE_MODE || BLE_HID_TIKTOK_MODE)
#endif // LE_HID_CONSUMER

#if LE_HID_DIGITIZER
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x04,        // Usage (Touch Screen)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x22,        //   Usage (Finger)
    0xA1, 0x02,        //   Collection (Logical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x32,        //     Usage (In Range)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x47,        //     Usage (0x47)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x51,        //     Usage (0x51)
    0x75, 0x05,        //     Report Size (5)
    0x95, 0x01,        //     Report Count (1)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x05,        //     Logical Maximum (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x75, 0x0C,        //     Report Size (12)
    0x95, 0x01,        //     Report Count (1)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x33,        //     Unit (System: English Linear, Length: Inch)
    0x26, 0xE8, 0x03,  //     Logical Maximum (1000)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //     Usage (Y)
    0x75, 0x0C,        //     Report Size (12)
    0x95, 0x01,        //     Report Count (1)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x33,        //     Unit (System: English Linear, Length: Inch)
    0x26, 0xE8, 0x03,  //     Logical Maximum (1000)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
#endif // LE_HID_DIGITIZER

#if LE_HID_MOUSE
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x05,        //     Report Count (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x03,        //     Report Size (3)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x38,        //     Usage (Wheel)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
#endif

#if LE_HID_KEYBOARD
	0x05, 0x01, 		//  USAGE_PAGE (Generic Desktop)
	0x09, 0x06, 		//  USAGE (Keyboard)
	0xA1, 0x01, 		//  COLLECTION (Application)
	0x85, 0x05, 		//	Report Id (5)
	0x05, 0x07, 		//	USAGE_PAGE (Keyboard)
	0x19, 0xE0, 		//	USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xE7, 		//	USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00, 		//	LOGICAL_MINIMUM (0)
	0x25, 0x01, 		//	LOGICAL_MAXIMUM (1)
	0x75, 0x01, 		//	REPORT_SIZE (1)
	0x95, 0x08, 		//  REPORT_COUNT (8)
	0x81, 0x02, 		//	INPUT (Data,Var,Abs)
	0x95, 0x01, 		//	REPORT_COUNT (1)
	0x75, 0x08, 		//	REPORT_SIZE (8)
	0x15, 0x00, 		//	LOGICAL_MINIMUM (0)
	0x25, 0x65, 		//	LOGICAL_MAXIMUM (101)
	0x05, 0x07, 		//	USAGE_PAGE (Keyboard)
	0x19, 0x00, 		//	USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65, 		//	USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00, 		//	INPUT (Data,Ary,Abs)
	0xC0,				//  END_COLLECTION
#endif // LE_HID_KEYBOARD
};

const u8 report_map_ios[]={
#if LE_HID_CONSUMER
#if (BLE_HID_SELFIE_MODE || BLE_HID_TIKTOK_MODE)
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x09, 0x30,        //   Usage (Power)
    0x09, 0x40,        //   Usage (Menu)
    0x09, 0x82,        //   Usage (Mode Step)
    0x09, 0xA0,        //   Usage (VCR Plus)
    0x09, 0xB5,        //   Usage (Scan Next Track)
    0x09, 0xB6,        //   Usage (Scan Previous Track)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x09, 0xE2,        //   Usage (Mute)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x0A, 0x83, 0x01,  //   Usage (AL Consumer Control Configuration)
    0x0A, 0x96, 0x01,  //   Usage (AL Internet Browser)
    0x0A, 0x23, 0x02,  //   Usage (AC Home)
    0x0A, 0x24, 0x02,  //   Usage (AC Back)
    0x0A, 0x2D, 0x02,  //   Usage (AC Zoom In)
    0x0A, 0x2E, 0x02,  //   Usage (AC Zoom Out)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x29, 0x01,        //   Usage Maximum (Consumer Control)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
#else
	0x05, 0x0C, 		//  Usage Page (Consumer)
	0x09, 0x01, 		//  Usage (Consumer Control)
	0xA1, 0x01, 		//  Collection (Application)
	0x85, 0x01, 		//	Report Id (1)
	0x1A, 0x00, 0x00,	//	Usage Minimum (Unassigned)
	0x2A, 0x9C, 0x02,	//	Usage Maximum (AC Distribute Vertically)
	0x16, 0x00, 0x00,	//	Logical minimum (0)
	0x26, 0x9C, 0x02,	//	Logical maximum (668)
	0x75, 0x10, 		//	Report Size (16)
	0x95, 0x01, 		//	Report Count (1)
	0x81, 0x00, 		//	Input (Data,Array,Absolute,Bit Field)
	0xC0,				//  End Collection
#endif //(BLE_HID_SELFIE_MODE || BLE_HID_TIKTOK_MODE)
#endif // LE_HID_CONSUMER

#if LE_HID_DIGITIZER
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x05,        // Usage (Touch Pad)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x22,        //   Usage (Finger)
    0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x09, 0x47,        //     Usage (0x47)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x25, 0x05,        //     Logical Maximum (5)
    0x09, 0x38,        //     Usage (Transducer Index)
    0x75, 0x06,        //     Report Size (6)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x46, 0xDC, 0x05,  //     Physical Maximum (1500)
    0x35, 0x00,        //     Physical Minimum (0)
    0x26, 0xA9, 0x0A,  //     Logical Maximum (2729)
    0x75, 0x0C,        //     Report Size (12)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0xBC, 0x02,  //     Physical Maximum (700)
    0x26, 0x54, 0x05,  //     Logical Maximum (1364)
    0x09, 0x31,        //     Usage (Y)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x22,        //   Usage (Finger)
    0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x09, 0x47,        //     Usage (0x47)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x25, 0x05,        //     Logical Maximum (5)
    0x09, 0x38,        //     Usage (Transducer Index)
    0x75, 0x06,        //     Report Size (6)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x46, 0xDC, 0x05,  //     Physical Maximum (1500)
    0x35, 0x00,        //     Physical Minimum (0)
    0x26, 0xA9, 0x0A,  //     Logical Maximum (2729)
    0x75, 0x0C,        //     Report Size (12)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0xBC, 0x02,  //     Physical Maximum (700)
    0x26, 0x54, 0x05,  //     Logical Maximum (1364)
    0x09, 0x31,        //     Usage (Y)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x22,        //   Usage (Finger)
    0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x09, 0x47,        //     Usage (0x47)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x25, 0x05,        //     Logical Maximum (5)
    0x09, 0x38,        //     Usage (Transducer Index)
    0x75, 0x06,        //     Report Size (6)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x46, 0xDC, 0x05,  //     Physical Maximum (1500)
    0x35, 0x00,        //     Physical Minimum (0)
    0x26, 0xA9, 0x0A,  //     Logical Maximum (2729)
    0x75, 0x0C,        //     Report Size (12)
    0x55, 0x0E,        //     Unit Exponent (-2)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0xBC, 0x02,  //     Physical Maximum (700)
    0x26, 0x54, 0x05,  //     Logical Maximum (1364)
    0x09, 0x31,        //     Usage (Y)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0x05, 0x09,        //   Usage Page (Button)
    0x25, 0x01,        //   Logical Maximum (1)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x02,        //   Usage Maximum (0x02)
    0x95, 0x02,        //   Report Count (2)
    0x75, 0x01,        //   Report Size (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
#endif // LE_HID_DIGITIZER

#if LE_HID_MOUSE
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x05,        //     Report Count (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x03,        //     Report Size (3)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x38,        //     Usage (Wheel)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
#endif

#if LE_HID_KEYBOARD
	0x05, 0x01, 		// USAGE_PAGE (Generic Desktop)
	0x09, 0x06, 		// USAGE (Keyboard)
	0xA1, 0x01, 		// COLLECTION (Application)
	0x85, 0x05, 		//	   Report Id (5)
	0x05, 0x07, 		//	   USAGE_PAGE (Keyboard)
	0x19, 0xE0, 		//	   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xE7, 		//	   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00, 		//	   LOGICAL_MINIMUM (0)
	0x25, 0x01, 		//	   LOGICAL_MAXIMUM (1)
	0x75, 0x01, 		//	   REPORT_SIZE (1)
	0x95, 0x08, 		//	   REPORT_COUNT (8)
	0x81, 0x02, 		//	   INPUT (Data,Var,Abs)
	0x95, 0x01, 		//	   REPORT_COUNT (1)
	0x75, 0x08, 		//	   REPORT_SIZE (8)
	0x15, 0x00, 		//	   LOGICAL_MINIMUM (0)
	0x25, 0x65, 		//	   LOGICAL_MAXIMUM (101)
	0x05, 0x07, 		//	   USAGE_PAGE (Keyboard)
	0x19, 0x00, 		//	   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65, 		//	   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00, 		//	   INPUT (Data,Ary,Abs)
	0xC0,				// END_COLLECTION
#endif // LE_HID_KEYBOARD
};

// 连接蓝牙后的状态回调函数
static void ble_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    bool event_valid = true;

    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&ble_connect_handle, &param[7], 2);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint16_t con_handle;
            memcpy(&con_handle, &param[0], 2);
            ble_connect_handle = 0;
            ble_report_digitizer_client_config = 0;
            ble_report_mouse_client_config = 0;
            ble_report_consumer_client_config = 0;
            ble_report_keyboard_client_config = 0;
        } break;

		case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            uint8_t status = param[0];
            uint16_t handle = param[1] | (param[2] << 8);
            uint16_t interval = param[3] | (param[4] << 8);
            uint16_t latency = param[5] | (param[6] << 8);
            uint16_t timeout = param[7] | (param[8] << 8);
            printf("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
        } break;

        default:
            event_valid = false;
            break;
    }

    if (event_valid) {
        ble_set_service_pending(1);
    }
}

static uint16_t ble_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    u16 data_len = 0;
    u16 hid_report_map_len = 0;
    const u8 *p_hid_report_map;

    if (ble_hid_peer_device_is_ios()){
        hid_report_map_len = sizeof(report_map_ios);
        p_hid_report_map = report_map_ios;
    }
    else {
        hid_report_map_len = sizeof(report_map_android);
        p_hid_report_map = report_map_android;
    }

    if (attribute_handle == ATT_CHARACTERISTIC_2a4b_01_VALUE_HANDLE) {
        ble_set_service_pending(1);
        if (buffer) {
            data_len = (buffer_size < (hid_report_map_len - offset))? buffer_size: (hid_report_map_len - offset);
            memcpy(buffer, p_hid_report_map + offset, data_len);

            TRACE("ble_service_read_callback 0x%04x data[%d]: ", attribute_handle, buffer_size);
            TRACE_R(buffer, buffer_size);
            return data_len;
        }
        return hid_report_map_len;
    }

	return 0;
}

static int ble_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    bool event_valid = true;
    uint16_t conn_interval;
    uint16_t conn_latency;
    uint16_t conn_timeout;

    TRACE("ble_service_write_callback 0x%04x data[%d]: ", attribute_handle, buffer_size);
    TRACE_R(buffer, buffer_size);

    if(attribute_handle == ATT_CHARACTERISTIC_2a4d_01_CLIENT_CONFIGURATION_HANDLE){
        ble_report_consumer_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a4d_02_CLIENT_CONFIGURATION_HANDLE) {
        ble_report_digitizer_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a4d_03_CLIENT_CONFIGURATION_HANDLE) {
        ble_report_mouse_client_config = GET_LE16(&buffer[0]);
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a4d_04_CLIENT_CONFIGURATION_HANDLE) {
    } else if (attribute_handle == ATT_CHARACTERISTIC_2a4d_05_CLIENT_CONFIGURATION_HANDLE) {
        ble_report_keyboard_client_config = GET_LE16(&buffer[0]);
    } else {
        event_valid = false;
    }

    if (event_valid) {
        ble_get_conn_param(con_handle, &conn_interval, &conn_latency, &conn_timeout);

        /***目前测到华为畅享9和华为荣耀play两部手机发现从机请求的连接参数中latency不为0时，会更新一个20s的超时时间，
            如果样机断电会出现手机超时断连很慢，如果在意该问题则把latency改为0，latency为0睡眠功耗会高很多。***/
        if ((conn_interval != 24) || (conn_latency == 0)) {
    		ble_update_conn_param(con_handle, 24, 20, 200);  // BLE_HID_POLLING_INTERVAL_MS / 1.25 ms
        }
        // if (conn_interval != 24) {
    	// 	// ble_update_conn_param(con_handle, 24, 0, 200);  // BLE_HID_POLLING_INTERVAL_MS / 1.25 ms
        // }

        ble_set_service_pending(1);
    }

	return ATT_ERR_NO_ERR;
}

int ble_hid_report_for_handle_kick(u16 handle, u8 *buffer, u8 len, uint8_t kick_cfg)
{
    u8 client_config_for_android = 0;
    u16 ble_report_client_config = 0;

    client_config_for_android = ((PEER_DEVICE_TYPE_ANDROID == ble_hid_peer_device_type_get()) &&
                                (ble_connect_handle != 0));

    switch (handle) {
        case ATT_CHARACTERISTIC_2a4d_01_VALUE_HANDLE:
            ble_report_client_config = ble_report_consumer_client_config;
            break;

        case ATT_CHARACTERISTIC_2a4d_02_VALUE_HANDLE:
            ble_report_client_config = ble_report_digitizer_client_config;
            break;

        case ATT_CHARACTERISTIC_2a4d_03_VALUE_HANDLE:
            ble_report_client_config = ble_report_mouse_client_config;
            break;

        case ATT_CHARACTERISTIC_2a4d_04_VALUE_HANDLE:
			break;

        case ATT_CHARACTERISTIC_2a4d_05_VALUE_HANDLE:
            ble_report_client_config = ble_report_keyboard_client_config;
			break;

        default:
            ble_report_client_config = 0;
            break;
    }

    if (ble_report_client_config || client_config_for_android) {
        TRACE("handle 0x%04x data_len:%d\n", handle, len);
        TRACE_R(buffer, len);
        return ble_notify_for_handle_kick(ble_connect_handle, handle, buffer, len, kick_cfg);
    }

    return -1;
}

int ble_hid_report_for_handle(u16 handle, u8 *buffer, u8 len)
{
    return ble_hid_report_for_handle_kick(handle, buffer, len, TXPKT_KICK_ENABLE);
}

void ble_hid_service_init(void)
{
    printf("%s\n", __func__);

	// 后面读写回调函数由这两个起始位置决定是否能用
    ble_service.start_handle   = ATT_SERVICE_1812_START_HANDLE;
    ble_service.end_handle     = ATT_SERVICE_1812_END_HANDLE;
	ble_service.read_callback  = &ble_service_read_callback;
	ble_service.write_callback = &ble_service_write_callback;
	ble_service.event_handler  = &ble_service_event_callback;
	att_server_register_service_handler(&ble_service);

    ble_report_digitizer_client_config = 0;
    ble_report_mouse_client_config = 0;
    ble_report_consumer_client_config = 0;
    ble_report_keyboard_client_config = 0;

	memset(&ble_hid_tasks, 0x00, sizeof(ble_hid_tasks_typedef));
}

bool ble_hid_task_enqueue(BLE_HID_CMD_ID_TYPEDEF id, u8 *buffer, u8 len)
{
    u8 next_head = (ble_hid_tasks.head_idx + 1) % BLE_HID_CMD_QUEUE_SIZE;

    if (next_head == ble_hid_tasks.rear_idx) {
        /* Queue is full, do nothing. */
        TRACE("####Queue full###!!\n");
        return false;
    } else {
        ble_hid_tasks.queue[ble_hid_tasks.head_idx].id = id;
        ble_hid_tasks.queue[ble_hid_tasks.head_idx].len = len;
        memcpy(ble_hid_tasks.queue[ble_hid_tasks.head_idx].buffer, buffer, len);
        ble_hid_tasks.head_idx = next_head;
    }

    return true;
}

// 循环中不断执行
void ble_hid_service_proc(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, BLE_HID_POLLING_INTERVAL_MS)) {

        if (ble_hid_tasks.head_idx != ble_hid_tasks.rear_idx) {
            delay_cnt = tick_get();
            ble_hid_cmd_typedef *ble_hid_cmd = &ble_hid_tasks.queue[ble_hid_tasks.rear_idx];

            switch (ble_hid_cmd->id) {
#if LE_HID_CONSUMER
                case BLE_HID_CMD_ID_CONSUMER:
                    TRACE("[TASK] consumer report.\n");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    if ((ble_hid_cmd->buffer[0] != 0xff) && (ble_hid_cmd->buffer[1] != 0xff)) {
                        ble_hid_report_for_handle(ATT_CHARACTERISTIC_2a4d_01_VALUE_HANDLE, ble_hid_cmd->buffer, ble_hid_cmd->len);
                    }
                    break;
#endif // LE_HID_CONSUMER

#if LE_HID_DIGITIZER
                case BLE_HID_CMD_ID_DIGITIZER:
                    TRACE("[TASK] digitizer report.\n");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    ble_hid_report_for_handle(ATT_CHARACTERISTIC_2a4d_02_VALUE_HANDLE, ble_hid_cmd->buffer, ble_hid_cmd->len);
                    break;
#endif // LE_HID_DIGITIZER

#if LE_HID_MOUSE
                case BLE_HID_CMD_ID_MOUSE:
                    TRACE("[TASK] mouse report.\n");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    ble_hid_report_for_handle(ATT_CHARACTERISTIC_2a4d_03_VALUE_HANDLE, &ble_hid_cmd->buffer, ble_hid_cmd->len);
                    break;
#endif // LE_HID_MOUSE

#if LE_HID_KEYBOARD
                case BLE_HID_CMD_ID_KEYBOARD:
                    TRACE("[TASK] keyboard report.\n");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    ble_hid_report_for_handle(ATT_CHARACTERISTIC_2a4d_05_VALUE_HANDLE, ble_hid_cmd->buffer, ble_hid_cmd->len);
#endif // LE_HID_KEYBOARD

                default:
                    break;
            }

            ble_hid_tasks.rear_idx = (ble_hid_tasks.rear_idx + 1) % BLE_HID_CMD_QUEUE_SIZE;
        }
    }
}
