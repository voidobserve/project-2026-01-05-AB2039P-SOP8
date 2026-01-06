/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


STRONG u8 mem_heap[512*6] AT(.osmem);

static const struct thread_cfg_cb thread_cfg =
{
    /*                  Enable      Priority    Stack   */
   .bt_stack   =        {0,          23,         1024},
   .timer      =        {1,          16,         512},
   .main       =        {1,          25,         1024},
   .dirver     =        {1,          17,         512},
};

STRONG uint32_t mem_heap_size_get(void)
{
   return sizeof(mem_heap);
}

STRONG struct thread_cfg_cb* thread_cfg_get(void)
{
   return (struct thread_cfg_cb*)&thread_cfg;
}

STRONG void thread_create_callback(void)
{
    if (thread_cfg.timer.enable) {
        thread_timer_create();
    }

    if (thread_cfg.dirver.enable) {
        thread_driver_create();
    }

    if (thread_cfg.bt_stack.enable) {
        thread_btstack_create();
    }
}
