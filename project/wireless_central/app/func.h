#ifndef _FUNC_H
#define _FUNC_H

#include "func_wireless.h"
#include "func_iodm.h"
#include "func_idle.h"
#include "func_lowpwr.h"


//task number
enum {
    FUNC_NULL = 0,
    FUNC_WIRELESS,
    FUNC_IODM,
    FUNC_PWROFF,
    FUNC_IDLE,
};

//task control block
typedef struct {
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
} func_cb_t;
extern func_cb_t func_cb;

void func_process(void);
void func_message(u16 msg);

void func_run(void);
void func_idle(void);
void func_wireless(void);
void func_iodm(void);

#endif
