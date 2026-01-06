#ifndef _BSP_KEYSCAN_H
#define _BSP_KEYSCAN_H


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

void bsp_keyscan_init(void);
uint8_t bsp_get_keyscan_id(void);

#endif // _BSP_KEYSCAN_H
