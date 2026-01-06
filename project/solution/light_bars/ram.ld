#include "config.h"

ENTRY(_start)

//Define the flash max size
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

__cache_stack_vma   = 0x10600;  //for loader
__cache_stack_size  = 512;      //for loader
__stack_vma         = 0x10800;
__stack_ram_size    = 1k;
__sram_vma          = 0x10c00;
__sram_ram_size     = 21k;
__comm_vma          = __comm_start;
//overlay
__pdn_comm_ram_size = 1024;
__pdn_comm_vma      = __sram_vma;


MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size

    stack           : org = __stack_vma,        len = __stack_ram_size
    sram            : org = __sram_vma,         len = __sram_ram_size

    pdn_comm        : org = __pdn_comm_vma,     len = __pdn_comm_ram_size
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.btmem.bthw)
        *(.btmem*)
        *(.osmem*)
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.ble_cache*)
        *(.ble_buf*)
        *(.fot_data.*)
        *(.tbox.*)
        *(.ledc*)
        *(.prod_test*)
        *(.fcc.huart_buf)
        *(.hci.h3*)
        *(.notch_iir.*)
        *(.thr_track_flash.*)
        . = ALIGN(256);
        __bss_end = .;
    } > sram
    __bss_size = __bss_end - __bss_start;

    .comm : {
        __comm_start = .;
        *(.vector)
        *(.com_text*)
        *(.com_rodata*)
        *debug.o(.rodata*)
        *(.com_periph.*)
        #if SYS_SLEEP_EN
        *(.com_sleep*)
        #endif
        *(.data*)
        *(.sdata*)
        . = ALIGN(512);
        __comm_end = .;
    } > sram AT > flash
    __comm_size = __comm_end - __comm_start;

    .pdn_comm : {
        *(.com_pwroff*)
        . = ALIGN(512);
    } > pdn_comm AT > flash

    .flash : {
         __code_start_app_proc = .;
        *(.text.app.proc.*)
        . = ALIGN(512);
        __code_end_app_proc = .;

        __code_start_stack_send = .;
        *(.text.stack.send.*)
        . = ALIGN(512);
        __code_end_stack_send = .;

        #if !SYS_SLEEP_EN
        __code_start_sleep_send = .;
        *(.com_sleep*)
        . = ALIGN(512);
        __code_end_sleep_send = .;
        #endif

        *(.thr_track_flash_text.*)
        *(.notch_iir_text.*)

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
}

//Calc the lma
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__pdn_comm_lma = LOADADDR(.pdn_comm);
__pdn_comm_size = SIZEOF(.pdn_comm);
