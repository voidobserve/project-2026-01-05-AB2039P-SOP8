#include "config.h"

ENTRY(_start)

//Define the flash max size
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

// Define the area size !!(temporary configuration)!!
__cache_stack_vma   = 0x10600;   //for loader
__cache_stack_size  = 512;       //for loader
__stack_vma         = 0x10800;
__stack_ram_size    = 1k;
__data_vma          = 0x10c00;
__data_ram_size     = 4k;
__comm_vma          = 0x11c00;
__comm_ram_size     = 8k;
__bram_vma          = 0x13c00;
__bram_ram_size     = 9k;


MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    comm(rx)        : org = __comm_vma,         len = __comm_ram_size

    stack           : org = __stack_vma,        len = __stack_ram_size
    data            : org = __data_vma,         len = __data_ram_size
    bram            : org = __bram_vma,         len = __bram_ram_size
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
        //*(.com_sleep*)
        *(.com_periph.*)
        *(.data*)
        *(.sdata*)
        . = ALIGN(512);
    } > comm AT > flash

    .flash : {
         __code_start_app_proc = .;
        *(.text.app.proc.*)
        . = ALIGN(512);
        __code_end_app_proc = .;

        __code_start_stack_send = .;
        *(.text.stack.send.*)
        . = ALIGN(512);
        __code_end_stack_send = .;

        __code_start_usbdev = .;
        *(.usbdev*)
        . = ALIGN(512);
        __code_end_usbdev = .;

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
         __heap_start = .;
        *(.osmem)
        __heap_end = .;

        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.btmem.stack.l2cap)
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;
    __heap_size = __heap_end - __heap_start;

    .bram __bram_vma (NOLOAD) : {
        __bram_start = .;
        *(.btmem.bthw)
        *(.btmem*)

        *(.ble_cache*)
        *(.ble_buf*)
        *(.fot_data.*)
        *(.tbox.*)
        *(.usb_buf*)
        __bram_end = .;
    } > bram
    __bram_size = __bram_end - __bram_start;

   //  .bram_dut __bram_vma (NOLOAD) : {
   //      . = 0x7000;
   //       *(.fcc.huart_buf)
   //      *(.hci.h3*)
   //      *(.prod_test*)
   // } > bram
}

//Calc the lma
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
