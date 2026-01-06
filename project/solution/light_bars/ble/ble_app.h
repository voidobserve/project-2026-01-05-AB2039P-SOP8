/**
******************************************************************************************************************************
*
*@file ble_app.h
*
*@brief Head file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef __BLE_APP_H
#define __BLE_APP_H

#include "include.h"

typedef struct
{
    uint8_t len;
    uint8_t type;
    uint8_t data[31 - 1];               //ADV_DATA_LEN - sizeof(len)
}__attribute__ ((__packed__)) ble_ltv_t;

#define REMOTE_TYPE_LAMP_CONTROLLER         0x01
#define REMOTE_TYPE_MUSIC_CONTROLLER        0x02
#define DEV_TYPE                            REMOTE_TYPE_MUSIC_CONTROLLER

#define TX_RETRY_CNT_MAX                    (7)
#define RETRY_CNT_CHECK(a)                  ((a) > TX_RETRY_CNT_MAX) ? TX_RETRY_CNT_MAX : (a)

#if (DEV_TYPE == REMOTE_TYPE_MUSIC_CONTROLLER)
#define VENDOR_CID                      (0x004c)
#define REMOTE_FILTER_CODE              (0xbe241502)

typedef enum
{
    REMOTE_CMD_2H               = 0x01,     //!< Timing Off Mode
    REMOTE_CMD_OFF              = 0x02,     //!< Turn Off
    REMOTE_CMD_AUTO             = 0x03,     //!< Turn On Auto
    REMOTE_CMD_1                = 0x04,     //!< Music Activation Mode
    REMOTE_CMD_2                = 0x05,     //!< Music Activation Mode
    REMOTE_CMD_1H               = 0x06,     //!< Timing Off Mode
    REMOTE_CMD_WHITE            = 0x07,     //!< 9 RGB Colors  -White
    REMOTE_CMD_ON               = 0x08,     //!< Turn On
    REMOTE_CMD_BRIGHTNESS_INC   = 0x09,     //!< Brightness Adujustment
    REMOTE_CMD_BRIGHTNESS_DEC   = 0x0A,     //!< Brightness Adujustment
    REMOTE_CMD_B                = 0x0B,     //!< 9 RGB Colors  -B
    REMOTE_CMD_BLUE             = 0x0C,     //!< 9 RGB Colors  -Blue
    REMOTE_CMD_PURPLE           = 0x0D,     //!< 9 RGB Colors  -Purple
    REMOTE_CMD_3H               = 0x0E,     //!< Timing Off Mode
    REMOTE_CMD_MODE_INC         = 0x0F,     //!< Dynamic Mode Adujustment
    REMOTE_CMD_MODE_DEC         = 0x10,     //!< Dynamic Mode Adujustment
    REMOTE_CMD_G                = 0x11,     //!< 9 RGB Colors  -G
    REMOTE_CMD_GREEN            = 0x12,     //!< 9 RGB Colors  -Green
    REMOTE_CMD_RED              = 0x13,     //!< 9 RGB Colors  -Red
    REMOTE_CMD_3                = 0x14,     //!< Music Activation Mode
    REMOTE_CMD_SPEED_INC        = 0x15,     //!< Speed Adujustment
    REMOTE_CMD_SPEED_DEC        = 0x16,     //!< Speed Adujustment
    REMOTE_CMD_R                = 0x17,     //!< 9 RGB Colors  -R
    REMOTE_CMD_YELLOW           = 0x18,     //!< 9 RGB Colors  -Yellow
}__attribute__ ((__packed__)) remote_cmd_t;

typedef struct
{
    uint16_t cid;           //!< Company ID
    uint32_t filter_code;   //!< Filter Code
    uint8_t sno;            //!< Sequence Number
    uint8_t unknow2[3];     //!< Unknow
    remote_cmd_t cmd;       //!< Command
    uint8_t param;          //!< Command Param
}__attribute__ ((__packed__)) remote_beacon_t;
#elif (VENDOR_CID == VENDOR_ID_LAMP_CONTROLLER)
#define VENDOR_CID                      (0x015e)
#define REMOTE_FILTER_CODE              (0xef00)

typedef enum
{
    REMOTE_CMD_BRIGHTNESS_PLUS  = 0x05,     //!< Brightness Plus
    REMOTE_CMD_BRIGHTNESS_MINUS = 0x06,     //!< Brightness Minus
    REMOTE_CMD_MUSIC            = 0x07,     //!< Music
    REMOTE_CMD_ONOFF            = 0x08,     //!< On Off

    REMOTE_CMD_R                = 0x09,     //!< Static Color - Red
    REMOTE_CMD_G                = 0x0a,     //!< Static Color - Green
    REMOTE_CMD_B                = 0x0b,     //!< Static Color - Blue
    REMOTE_CMD_W                = 0x0c,     //!< Static Color - White
    REMOTE_CMD_PALE_RED         = 0x0d,     //!< Static Color - Pale Red
    REMOTE_CMD_PALE_GREEN       = 0x0e,     //!< Static Color - Pale Green
    REMOTE_CMD_PALE_BLUE        = 0x0f,     //!< Static Color - Pale Blue

    REMOTE_CMD_FLASH            = 0x10,     //!< Flash Mode

    REMOTE_CMD_ORANGE           = 0x11,     //!< Static Color - Orange
    REMOTE_CMD_INDIGO           = 0x12,     //!< Static Color - Indigo
    REMOTE_CMD_PURPLE           = 0x13,     //!< Static Color - Purple

    REMOTE_CMD_STROBE           = 0x14,     //!< Breath Mode

    REMOTE_CMD_ORANGE_YELLOW    = 0x15,     //!< Static Color - Orange Yellow
    REMOTE_CMD_INDIGO_BLUE      = 0x16,     //!< Static Color - Indigo Blue
    REMOTE_CMD_BLUE_PURPLE      = 0x17,     //!< Static Color - Blue Purple

    REMOTE_CMD_FADE             = 0x18,     //!< Gradient Mode

    REMOTE_CMD_YELLOW           = 0x19,     //!< Static Color - Yellow
    REMOTE_CMD_INDIGO_GREEN     = 0x1a,     //!< Static Color - Indigo Green
    REMOTE_CMD_PALE_PURPLE      = 0x1b,     //!< Static Color - Pale Purple

    REMOTE_CMD_SMOOTH           = 0x1c,     //!< Jump Mode
}__attribute__ ((__packed__)) remote_cmd_t;

typedef struct
{
    uint16_t cid;           //!< Company ID
    uint8_t cmd;            //!< Command
    uint8_t param;          //!< Key State; 1 press 0 release
    uint8_t sno;            //!< Sequence Number
    uint16_t random;
    uint16_t filter_code;   //!< Filter Code
}__attribute__ ((__packed__)) remote_beacon_t;

#endif

typedef struct {
    uint8_t ble_cmd_tx_cnt;
    uint8_t tx_data[31 - 1];
    uint8_t tx_len;
}ble_slave_tx_t;


void ble_app_env_init(void);
void ble_advertising_report_callback(uint8_t evt_type, uint8_t addr_type, uint8_t *addr, uint8_t adv_data_len, const uint8_t *adv_data, char rssi);

void ble_slave_tx_command(uint8_t *p, uint8_t len, uint8_t retry_cnt);
void ble_slave_tx_proc(void);

#endif
