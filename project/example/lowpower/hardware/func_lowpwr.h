#ifndef __FUNC_LOWPWR_H
#define __FUNC_LOWPWR_H

typedef bool (*is_sleep_func)(void);

bool sleep_process(is_sleep_func is_sleep);
void func_pwroff(void);

#endif // __FUNC_LOWPWR_H
