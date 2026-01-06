#ifndef _IR_TX_H
#define _IR_TX_H

#define IR_KEY_UP_CMD          0xC1
#define IR_KEY_DOWN_CMD        0xC2
#define IR_KEY_LEFT_CMD        0xC3
#define IR_KEY_RIGHT_CMD       0xC4
#define IR_KEY_MEUN_CMD        0xCA
#define IR_KEY_OK_CMD          0xC0
#define IR_KEY_BACK_CMD        0xD4
#define IR_KEY_VOL_UP_CMD      0x04
#define IR_KEY_VOL_DOWN_CMD    0x00
#define IR_KEY_PWR_CMD         0x07
#define IR_KEY_NONE_CMD        0xFF


#define BLE_HID_CMD_BUF_LEN            4
#define BLE_HID_CMD_BUF_MASK           (BLE_HID_CMD_BUF_LEN - 1)
#define BLE_HID_RX_BUF_LEN             20

#define BLE_HID_NOTIFY_DATA_BUF_LEN    3

void ir_tx_init(void);
void ir_tx_proc(void);

#endif  //_IR_TX_H
