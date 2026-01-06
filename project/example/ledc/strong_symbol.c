/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

u8 mem_heap[512*5] AT(.osmem);

static const struct thread_cfg_cb thread_cfg =
{
   .bt_stack   =       {1, 23, 1024},
   .timer      =       {0, 16, 512},
   .main       =       {1, 25, 512},
};


uint32_t mem_heap_size_get(void)
{
   return sizeof(mem_heap);
}

struct thread_cfg_cb* thread_cfg_get(void)
{
   return (struct thread_cfg_cb*)&thread_cfg;
}
