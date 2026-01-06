#ifndef _BSP_SYS_H
#define _BSP_SYS_H


typedef struct {
    u32 rst_reason;
    u32 wakeup_reason;
#if USB_EN
    u8 usb_is_active;
#endif
} sys_cb_t;
extern sys_cb_t sys_cb;

void bsp_sys_init(void);
void bsp_periph_init(void);
void sys_ram_info_dump(void);
void usr_tmr5ms_callback(void);

#endif // _BSP_SYS_H

