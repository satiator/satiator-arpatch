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


int is_satiator_present(void) {
    const cmd_t cmd_info = {0x0100, 0, 0, 0};
    exec_cmd(cmd_info, 0);
    int hw_flag = CDB_REG_CR2 >> 8;
    if (!(hw_flag & 2))
        return 0;

    const cmd_t cmd_auth = {0xe000, 0x0001, 0, 0};
    exec_cmd(cmd_auth, HIRQ_MPED);

    const cmd_t cmd_info = {0x0100, 0, 0, 0};
    exec_cmd(cmd_info, 0);
    return (CDB_REG_CR3 & 0xff) == 3;
}

void boot_satiator(void) {
    s_mode(s_api);
    int fd = s_open("menu.bin", FA_READ);
    s_seek(fd, 0x1000, C_SEEK_SET);
    uint8_t *dest = (void*)0x200000;
    uint8_t *end = dest + 0x1000;
    while (dest < end) {
        s_read(fd, dest, S_MAXBUF);
        dest += S_MAXBUF;
    }

    ((void(*)(void))0x200000)();
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}

size_t strlen(const char *src) {
    size_t n = 0;
    while (*src++)
        n++;
    return n;
}

void boot_ar(void);
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
