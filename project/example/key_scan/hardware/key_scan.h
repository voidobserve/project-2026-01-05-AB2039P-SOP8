#ifndef _KEY_SCAN_H
#define _KEY_SCAN_H

#include "include.h"
#include "driver_keyscan.h"
#include "driver_gpio.h"

/* Matrix Mode
 -                                                  KI0    KJ0
 - KA1                                              KI1    KJ1
 - KA2    KB2                                       KI2    KJ2
 - KA3    KB3    KC3                                KI3    KJ3
 - KA4    KB4    KC4    KD4                         KI4    KJ4
 - KA5    KB5    KC5    KD5    KE5                  KI5    KJ5
 - KA6    KB6    KC6    KD6    KE6    KF6           KI6    KJ6
 - KA7    KB7    KC7    KD7    KE7    KF7    KG7    KI7    KJ7
 */

/* Normal Mode
 -                             KI0    KJ0
 -                             KI1    KJ1
 -                             KI2    KJ2
 -                             KI3    KJ3
 - KA4    KB4    KC4    KD4    KI4    KJ4
 - KA5    KB5    KC5    KD5    KI5    KJ5
 - KA6    KB6    KC6    KD6    KI6    KJ6
 - KA7    KB7    KC7    KD7    KI7    KJ7
 */

#define INTERRUPT_EN        1

// Key ID
#define KEY_ID_NO                0x00
#define KEY_ID_K1                0x01
#define KEY_ID_K2                0x02
#define KEY_ID_K3                0x03
#define KEY_ID_K4                0x04
#define KEY_ID_K5                0x05
#define KEY_ID_K6                0x06
#define KEY_ID_K7                0x07
#define KEY_ID_K8                0x08
#define KEY_ID_K9                0x09
#define KEY_ID_K10               0x0a
#define KEY_ID_K11               0x0b
#define KEY_ID_K12               0x0c
#define KEY_ID_K13               0x0d
#define KEY_ID_K14               0x0e
#define KEY_ID_K15               0x0f
#define KEY_ID_K16               0x10
#define KEY_ID_K17               0x11
#define KEY_ID_K18               0x12
#define KEY_ID_K19               0x13
#define KEY_ID_K20               0x14
#define KEY_ID_K21               0x15
#define KEY_ID_K22               0x16
#define KEY_ID_K23               0x17
#define KEY_ID_K24               0x18
#define KEY_ID_K25               0x19
#define KEY_ID_K26               0x1a
#define KEY_ID_K27               0x1b
#define KEY_ID_K28               0x1c
#define KEY_ID_K29               0x1d
#define KEY_ID_K30               0x1e
#define KEY_ID_K31               0x1f
#define KEY_ID_K32               0x20
#define KEY_ID_K33               0x21
#define KEY_ID_K34               0x22
#define KEY_ID_K35               0x23
#define KEY_ID_K36               0x24
#define KEY_ID_K37               0x25
#define KEY_ID_K38               0x26
#define KEY_ID_K39               0x27
#define KEY_ID_K40               0x28
#define KEY_ID_K41               0x29
#define KEY_ID_K42               0x2a
#define KEY_ID_K43               0x2b
#define KEY_ID_K44               0x2c

void key_scan_init(void);
void key_scan_example(void);

#endif // _KEY_SCAN_H
