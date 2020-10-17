OUTPUT_FORMAT("elf32-sh", "elf32-sh",
          "elf32-sh")
OUTPUT_ARCH(sh)
ENTRY(_start)

MEMORY {
    cart_trampoline (rx) : org = 0x02000f00, l = 0x40
    cart_main (rx) : org = 0x02020000, l = 0x20000
    work_low (rwx) : org = 0x200000, l = 0x100000
}

SECTIONS {
    .trampoline : {
        out/trampoline.o
    } >cart_trampoline

    .cart_header : {

        __load_from = .;
    } >cart_main

    .ram : AT(__load_from) {
        __load_start = .;
        *(.text);
        *(.text.*);
        *(.rodata);
        *(.rodata.*);
        *(.data);
        *(.data.*);
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
