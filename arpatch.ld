OUTPUT_FORMAT("elf32-sh", "elf32-sh",
          "elf32-sh")
OUTPUT_ARCH(sh)
ENTRY(_start)

MEMORY {
    cart_trampoline (rx) : org = 0x02000f00, l = 0x40
    cart_main (rx) : org = 0x020160a0, l = 0x20000 - 0x160a0
    /* stay above the Satiator's load region, and below the AR's */
    work_low (rwx) : org = 0x220000, l = 0x0e0000
}

SECTIONS {
    .trampoline : {
        KEEP(out/trampoline.o)
    } >cart_trampoline

    .cart_header : {
        /* header must be aligned to 0x100 */
        . = ALIGN(0x100);
        KEEP(*(.header))

        *(.text);
        *(.text.*);
        *(.rodata);
        *(.rodata.*);

        . = ALIGN(4);
        __load_from = .;
    } >cart_main

    .ram : AT(__load_from) {
        __load_start = .;
        *(.data);
        *(.data.*);
        . = ALIGN(4);
        __load_end = .;

        *(.bss);
        *(.bss.*);
        *(COMMON);
        __bss_end = .;
        . = ALIGN(4);
        __free_ram_begin = .;
        __free_ram_end = __load_start + 0x100000;
    } >work_low

    .free : {
        __free_flash_begin = .;
    } >cart_main
}
