                mov.l   load_start, r0
                mov.l   load_from, r1
                mov.l   load_end, r2

more:
                mov.w   @r1, r3
                mov.w   r3, @r0
                add     #2, r0
                add     #2, r1
                cmp/hi     r2, r0
                bf      more
                nop
                mov.l   start, r0
                jmp     @r0
                nop

    .align 4
load_start:
    .long __load_start
load_from:
    .long __load_from
load_end:
    .long __load_end;
start:
    .long _start
