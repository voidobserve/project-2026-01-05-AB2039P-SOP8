#ifndef _WIRELESS_H
#define _WIRELESS_H

#define ALARM_TIMER_5MS_SCAN_EN             1

#define wireless_set_work_mode              ble_set_work_mode
#define wireless_get_work_mode              ble_get_work_mode

typedef struct PACKED {
    uint8_t         address_type;
    uint8_t         addr[6];
} wireless_addr_info_typedef;

void wireless_module_init(void);
void wireless_module_deinit(void);
void wireless_client_init(void);
void wireless_client_process(void);
void wireless_link_info_write(wireless_addr_info_typedef *addr_info);
bool wireless_link_info_read(wireless_addr_info_typedef *addr_info);

void wireless_enter_sleep_hook(void);
void wireless_exit_sleep_hook(void);

void soft_timer_5ms_disable(void);
void soft_timer_5ms_enable(void);

#endif // _WIRELESS_H
