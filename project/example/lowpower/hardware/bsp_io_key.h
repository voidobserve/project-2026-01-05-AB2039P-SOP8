#ifndef _BSP_IO_KEY_H
#define _BSP_IO_KEY_H
#include "driver_gpio.h"

// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05

void bsp_io_key_init(void);
u8 bsp_get_io_key_id(void);


#endif // _BSP_IO_KEY_H