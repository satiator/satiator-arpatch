// These routines stay in Flash so they can be called from running programs
#include <stdint.h>
#include <stddef.h>

extern const uint8_t binary_out_ar_original_trampoline_bin_start, binary_out_ar_original_trampoline_bin_end;

static void rom_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
}

void boot_ar(void) {
    // These are the offsets from the original AR code at 0x02000f00
    rom_memcpy((void*)0x280000, (void*)0x02000f00, 0x10000);
    // restore the original data too
    rom_memcpy((void*)0x280f00, &binary_out_ar_original_trampoline_bin_start, &binary_out_ar_original_trampoline_bin_end - &binary_out_ar_original_trampoline_bin_start);
    ((void(*)(void))0x288bec)();
}
