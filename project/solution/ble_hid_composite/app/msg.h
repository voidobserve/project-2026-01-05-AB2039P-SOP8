#ifndef _MSG_H
#define _MSG_H

#define MSG_TYPE_KEY_EVENT1         0x0000
#define MSG_TYPE_KEY_EVENT2         0x1000
#define MSG_TYPE_SYS_EVENT          0x7000
#define MSG_TYPE_HID_EVENT          0xf000
#define MSG_TYPE_MASK               0xf000

//Event Message, 范围：0x7700 ~ 0x77ff
enum{
    MSG_SYS_1S                      = 0x7700,
    MSG_SYS_500MS,
    
    MSG_SYS_MAX                     = 0x77ff,
};

#endif 