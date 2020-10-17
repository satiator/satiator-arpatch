#include <stdint.h>

extern uint32_t _load_end, _bss_end;

void start(void) {
    uint32_t *p = &_load_end;
    while (p < &_bss_end)
        *p++ = 0;

    for (;;);
}
