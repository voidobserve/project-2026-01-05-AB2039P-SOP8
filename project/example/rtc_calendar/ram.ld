#include "config.h"

ENTRY(_start)

//Define the flash max size
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

// Define the area size
__cache_stack_vma   = 0x10600;  //for loader
__cache_stack_size  = 512;      //for loader
__stack_vma         = 0x10800;
__stack_ram_size    = 1k;
__heap_vma          = 0x10c00;
__heap_ram_size     = 7k;
__data_vma          = 0x12800;
__data_ram_size     = 4k;
__comm_vma          = 0x13800;
__comm_ram_size     = 18k;
__bram_vma          = 0x18000;
__bram_ram_size     = 27k;
__cram_vma          = 0x1ec00;
__cram_ram_size     = 5k;


MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    comm(rx)        : org = __comm_vma,         len = __comm_ram_size

    stack           : org = __stack_vma,        len = __stack_ram_size
    heap            : org = __heap_vma,         len = __heap_ram_size
    data            : org = __data_vma,         len = __data_ram_size
    bram            : org = __bram_vma,         len = __bram_ram_size
    cram            : org = __cram_vma,         len = __cram_ram_size
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        *(.vector)
        *(.com_text*)
        *(.com_rodata*)
        *debug.o(.rodata*)
        *(.com_sleep*)
        *(.com_periph.*)
        *(.data*)
        *(.sdata*)
        . = ALIGN(512);
    } > comm AT > flash

    .flash : {

        __code_start_sleep = .;
        . = ALIGN(512);
        __code_end_sleep = .;

        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_start_fota = .;
        *(.text.fot.cache*)
        . = ALIGN(512);
        __code_end_fota = .;

        *(.irq_init.aligned)
        . = ALIGN(512);
        *(.text.ble.chmap*)
        . = ALIGN(512);
        *(.text*)
        *(.rodata*)
        *(.srodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);

    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x400;
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.btmem.stack.l2cap)
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;

    .heap (NOLOAD) : {
        *(.osmem)
    } > heap

    .bram __bram_vma (NOLOAD) : {
        *(.btmem.bthw)
        *(.btmem*)

        *(.ble_cache*)
        *(.ble_buf*)
    } > bram

    .cram_aac __cram_vma (NOLOAD) : {
        *(.fcc.huart_buf)
        *(.hci.h3*)
    } > cram

    .cram_fot __cram_vma (NOLOAD) : {
        *(.fot_data.*)
    } > cram
}

//Calc the lma
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
