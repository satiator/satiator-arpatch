    .global _ubc_isr
_ubc_isr:
    mov.l   r0, @-r15
    mov.l   r1, @-r15
    mov.l   r2, @-r15
    mov.l   r3, @-r15
    mov.l   r4, @-r15
    mov.l   r5, @-r15
    mov.l   r6, @-r15
    mov.l   r7, @-r15
    mov.l   r8, @-r15
    mov.l   r9, @-r15
    mov.l   r10, @-r15
    mov.l   r11, @-r15
    mov.l   r12, @-r15
    mov.l   r13, @-r15
    sts.l   pr, @-r15
    stc.l   gbr, @-r15
    sts.l   mach, @-r15
    sts.l   macl, @-r15

    mov r15, r4

    mov.l   ubc_handler, r0
    jsr     @r0
    nop

    lds.l   @r15+, macl
    lds.l   @r15+, mach
    ldc.l   @r15+, gbr
    lds.l   @r15+, pr
    mov.l   @r15+, r13
    mov.l   @r15+, r12
    mov.l   @r15+, r11
    mov.l   @r15+, r10
    mov.l   @r15+, r9
    mov.l   @r15+, r8
    mov.l   @r15+, r7
    mov.l   @r15+, r6
    mov.l   @r15+, r5
    mov.l   @r15+, r4
    mov.l   @r15+, r3
    mov.l   @r15+, r2
    mov.l   @r15+, r1
    mov.l   @r15+, r0

    rte
    nop

    .align 4

ubc_handler:
    .long   _ubc_handler
