#include <stdint.h>
#include <stddef.h>

extern uint32_t _load_end, _bss_end;

void boot(void);

void start(void) {
    uint32_t *p = &_load_end;
    while (p < &_bss_end)
        *p++ = 0;

    boot();
}

#include "cdblock.h"

typedef uint16_t cmd_t[4];

void exec_cmd(const cmd_t cr, uint16_t wait) {
    CDB_REG_HIRQ = ~(HIRQ_CMOK | wait);
    CDB_REG_CR1 = cr[0];
    CDB_REG_CR2 = cr[1];
    CDB_REG_CR3 = cr[2];
    CDB_REG_CR4 = cr[3];
    while (!(CDB_REG_HIRQ & HIRQ_CMOK));
    if (wait)
        while (!(CDB_REG_HIRQ & wait));
}

int is_satiator_present(void) {
    const cmd_t cmd_auth = {0xe000, 0x0001, 0, 0};
    exec_cmd(cmd_auth, HIRQ_MPED);

    const cmd_t cmd_info = {0x0100, 0, 0, 0};
    exec_cmd(cmd_info, 0);
    return (CDB_REG_CR3 & 0xff) == 2;
}

void boot_satiator(void) {
    const cmd_t cmd_get_mpeg_bios = {0xe200, 2, 0, 2};
    exec_cmd(cmd_get_mpeg_bios, HIRQ_MPED);
    const cmd_t cmd_get_then_delete_sector_data = {0x6300, 0, 0, 2};
    exec_cmd(cmd_get_then_delete_sector_data, HIRQ_EHST);

    while (!(CDB_REG_HIRQ & HIRQ_DRDY))
        ;

    void (*menu)(void) = (void*)0x200000;

    uint32_t *dest = (uint32_t*)menu;
    for (int i = 0; i < 0x400; i++)
        *dest++ = CDB_REG_DATATRNS;

    menu();
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}

extern const uint8_t binary_out_ar_original_trampoline_bin_start, binary_out_ar_original_trampoline_bin_end;

static void boot_ar(void) {
    // These are the offsets from the original AR code at 0x02000f00
    memcpy((void*)0x280000, (void*)0x02000f00, 0x10000);
    // restore the original data too
    memcpy((void*)0x280f00, &binary_out_ar_original_trampoline_bin_start, &binary_out_ar_original_trampoline_bin_end - &binary_out_ar_original_trampoline_bin_start);
    ((void(*)(void))0x288bec)();
}

void boot(void) {
    if (is_satiator_present()) {
        boot_satiator();
    } else {
        boot_ar();
    }
}
