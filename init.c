#include <stdint.h>
#include <stddef.h>
#include "satiator.h"

extern uint32_t _load_end, _bss_end;

void boot(void);

void start(void) {
    uint32_t *p = &_load_end;
    while (p < &_bss_end)
        *p++ = 0;

    boot();
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}


int is_satiator_present(void) {
    const cmd_t cmd_info = {0x0100, 0, 0, 0};
    exec_cmd(cmd_info, 0);
    int hw_flag = CDB_REG_CR2 >> 8;
    if (!(hw_flag & 2))
        return 0;

    const cmd_t cmd_auth = {0xe000, 0x0001, 0, 0};
    exec_cmd(cmd_auth, HIRQ_MPED);

    exec_cmd(cmd_info, 0);
    return (CDB_REG_CR3 & 0xff) == 3;
}

void boot_satiator(void) {
    s_mode(s_api);
    for (volatile int i=0; i<2000; i++)
        ;

    int (**bios_get_mpeg_rom)(uint32_t index, uint32_t size, uint32_t addr) = (void*)0x06000298;
    int ret = (*bios_get_mpeg_rom)(2, 2, 0x200000);

    ((void(*)(void))0x200000)();
}

void stop_slave_sh2(void);
void reset_to_satiator(void) {
    asm volatile (
        "mov    #0xf0, r0   \n\t"
        "ldc    r0, sr      \n\t"
        : : : "r0"
    );

    stop_slave_sh2();

    boot_satiator();
}

extern const uint8_t binary_out_ar_original_trampoline_bin_start, binary_out_ar_original_trampoline_bin_end;

void boot_ar(void) {
    // These are the offsets from the original AR code at 0x02000f00
    memcpy((void*)0x280000, (void*)0x02000f00, 0x10000);
    // restore the original data too
    memcpy((void*)0x280f00, &binary_out_ar_original_trampoline_bin_start, &binary_out_ar_original_trampoline_bin_end - &binary_out_ar_original_trampoline_bin_start);
    ((void(*)(void))0x288bec)();
}

size_t strlen(const char *src) {
    size_t n = 0;
    while (*src++)
        n++;
    return n;
}

int pad_read(void);

void boot(void) {
    int buttons = pad_read();

    if (buttons & 0x0400)  // A held
        boot_ar();

    if (is_satiator_present()) {
        boot_satiator();
    } else {
        boot_ar();
    }
}

void hook_install(void);

satiator_zone_header_t __attribute__((section(".header"))) header = {
    .signature = "SatiatorCart",
    .header_version = 1,
    .version_str = VERSION,
    .bootcode = boot_ar,
    .install_soft_reset_hook = hook_install,
};
