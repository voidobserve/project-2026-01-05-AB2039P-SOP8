#ifndef __BSP_PARAM_H
#define __BSP_PARAM_H

#define PARAM_BT_XOSC_CAP           (0x00)                                                  //1Byte
#define PARAM_BT_XOSC_CAP_LEN       1
#define PARAM_FOT_ADDR              (PARAM_BT_XOSC_CAP + PARAM_BT_XOSC_CAP_LEN)             //4BYTE
#define PARAM_FOT_ADDR_LEN          4
#define PARAM_FOT_REMOTE_VER        (PARAM_FOT_ADDR + PARAM_FOT_ADDR_LEN)                   //2BYTE
#define PARAM_FOT_REMOTE_VER_LEN    2
#define PARAM_FOT_HEAD_INFO         (PARAM_FOT_REMOTE_VER + PARAM_FOT_REMOTE_VER_LEN)       //8BYTE
#define PARAM_FOT_HEAD_INFO_LEN     8
#define PARAM_FOT_HASH              (PARAM_FOT_HEAD_INFO + PARAM_FOT_HEAD_INFO_LEN)         //4BYTE
#define PARAM_FOT_HASH_LEN          4

#define PARAM_LIGHT_MARK                (PARAM_FOT_HASH + PARAM_FOT_HASH_LEN)
#define PARAM_LIGHT_MARK_LEN            1
#define PARAM_LIGHT_MODE                (PARAM_LIGHT_MARK + PARAM_LIGHT_MARK_LEN)
#define PARAM_LIGHT_MODE_LEN            1
#define PARAM_LIGHT_AUTO_WEATHER        (PARAM_LIGHT_MODE + PARAM_LIGHT_MODE_LEN)
#define PARAM_LIGHT_AUTO_WEATHER_LEN    1
#define PARAM_LIGHT_BRIGHTNESS          (PARAM_LIGHT_AUTO_WEATHER + PARAM_LIGHT_AUTO_WEATHER_LEN)
#define PARAM_LIGHT_BRIGHTNESS_LEN      2
#define PARAM_LIGHT_TEMPERATURE         (PARAM_LIGHT_BRIGHTNESS + PARAM_LIGHT_BRIGHTNESS_LEN)
#define PARAM_LIGHT_TEMPERATURE_LEN     2
#define PARAM_LIGHT_WEATHER_INDEX       (PARAM_LIGHT_TEMPERATURE + PARAM_LIGHT_TEMPERATURE_LEN)
#define PARAM_LIGHT_WEATHER_INDEX_LEN   1


void bsp_param_init(void);
void bsp_param_write(u8 *buf, u32 addr, uint len);
void bsp_param_read(u8 *buf, u32 addr, uint len);
void bsp_param_sync(void);

//fota
void param_fot_addr_write(u8 *param);
void param_fot_addr_read(u8 *param);
void param_fot_remote_ver_write(u8 *param);
void param_fot_remote_ver_read(u8 *param);
void param_fot_head_info_write(u8 *param);
void param_fot_head_info_read(u8 *param);
void param_fot_hash_write(u8 *param);
void param_fot_hash_read(u8 *param);


#endif // __BSP_PARAM_H

