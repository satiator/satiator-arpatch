#include <stdint.h>

#define BARA        (*(volatile uint32_t *)0xFFFFFF40)
#define BAMRA       (*(volatile uint32_t *)0xFFFFFF44)
#define BBRA        (*(volatile uint16_t *)0xFFFFFF48)
#define BARB        (*(volatile uint32_t*)0xFFFFFF60)
#define BAMRB       (*(volatile uint32_t*)0xFFFFFF64)
#define BBRB        (*(volatile uint16_t*)0xFFFFFF68)
#define BDRB        (*(volatile uint32_t*)0xFFFFFF70)
#define BDMRB       (*(volatile uint32_t*)0xFFFFFF74)
#define BRCR        (*(volatile uint16_t*)0xFFFFFF78)

#define BBR_INSN ((1<<6) | (1<<4) | (1<<2))
#define BBR_DATA ((1<<6) | (2<<4) | (2<<2))

struct stack_frame {
    uint32_t macl, mach;
    uint32_t gbr, pr;
    uint32_t r13, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0;
    uint32_t pc, sr;
};

extern uint32_t _payload_start, _payload_size, _payload_target;
void reset_to_satiator(void);

void ubc_handler(struct stack_frame *frame) {


    BBRA = 0;
    *(void**)0x0600026C = reset_to_satiator;

    if (frame->pc == 0x44c) {
        frame->r0 = 0xe0;

        BARA = 0x0027fff0;
        BBRA = BBR_DATA;
    } else if (frame->pc == 0x2b0) {
        frame->pc = frame->pr;
        BARA = 0x0600002c;
        BBRA = BBR_DATA;
    } else if (frame->pc == 0x2c4) {
        frame->pc = frame->pr;
        BARA = 0x0600026c;
        BBRA = BBR_DATA;
    } else if (frame->pc == 0x060011C2) {
        BARA = 0x06000e24;
    } else {
        BBRA = BBR_DATA;
    }
}

void ubc_isr(void);

void hook_install(void) {
    BARA = 0x44a;
    BRCR = 0x1400;   // modern mode; break after insn
    BBRA = BBR_INSN;

    volatile uint32_t *vectab = (void*)(0x06000000);
    vectab[12] = (uint32_t)&ubc_isr;

    BARA = 0x44a;
    BRCR = 0x1400;   // modern mode; break after insn
    BBRA = BBR_INSN;
}
