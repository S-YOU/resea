#include "asmdefs.h"

.macro SAVE_REGS
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
.endm

.macro RESTORE_REGS
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
.endm

.extern handle_syscall
.globl x64_syscall_handler
x64_syscall_handler:
    /*
     *  RDI: type
     *  RSI: a1
     *  RDX: a2
     *  RCX: user RIP (to be exchanged with R13)
     *  R8:  a4
     *  R9:  a5
     *  R10: a6
     *  R11: user RFLAGS (to be exchanged with R14)
     *  R12: a8
     *  R13: a3
     *  R14: a7
     *
     */

    /* Switch to the kernel stack. */
    swapgs
    mov r15, rsp
    mov rsp, gs:[0]

    push r11 /* user RFLAGS */
    push r15 /* user RSP */
    push rcx /* user RIP */
    xchg rcx, r13
    xchg r11, r14

    call handle_syscall

    /* TODO: Sanitize caller-saved registers not to leak information to user. */

    pop rcx
    pop r11
    pop rsp
    swapgs
    sysret

.extern x64_handle_irq
.macro INTR_HANDLER num
.align 8
.globl x64_irq_handler\num
x64_irq_handler\num:
    cli

    /*
     * The current RSP points to IST0, a shared interrupt handler stack.
     * Switch into the thread's own kernel stack since x64_handler_irq()
     * returns after context switches to other threads.
     */

    push rsi
    push rax
    mov rsi, rsp

    /* Check CS register in the IRET frame. */
    test qword ptr [rsi + 24], 3
    jz 1f

    /* from user: use dedicated kernel stack */
    swapgs
    /* XXX: It seems that GS is overwritten. Why? */
    mov ax, 0x10
    mov gs, ax
    mov rsp, gs:[0] /* Extract RSP from gsinfo. */
    jmp 2f
1:
    /* from kernel: use current stack (assuming Red Zone is disabled) */
    mov rsp, [rsp + 40] /* Extract RSP from the IRET frame */
    jmp 2f

2:
    /* Copy IRET frame. */
    push [rsi + 48]    /* SS */
    push [rsi + 40]    /* RSP */
    push [rsi + 32]    /* RFLAGS */
    push [rsi + 24]    /* CS */
    push [rsi + 16]     /* RIP */
    mov rsi, [rsi + 8] /* Restore RSI. */
    mov rax, [rsi + 0] /* Restore RAX. */
    SAVE_REGS

    mov rdi, \num
    call x64_handle_irq
    jmp ret_from_irq
.endm

ret_from_irq:
    RESTORE_REGS

    /* Check CS register in the IRET frame. */
    test qword ptr [rsp + 8], 3
    jz 1f
    swapgs
1:
    iretq


.extern x64_handle_exception
.macro EXP_HANDLER_WITH_ERRCODE num
.align 8
.globl  x64_exp_handler\num
x64_exp_handler\num:
    cli

    /* Get error code from the stack and overwrite by SAVE_REGS. */
    push rax
    mov  rax, [rsp + 8] /* error code */
    mov  [rsp - 112], rax
    pop  rax
    add  rsp, 8

    SAVE_REGS

    mov rdi, \num
    mov rsi, [rsp - 8] /* error code */
    call x64_handle_exception
    jmp ret_from_exp
.endm

ret_from_exp:
    RESTORE_REGS
    iretq

.macro EXP_HANDLER_WITHOUT_ERRCODE num
.align 8
.globl x64_exp_handler\num
x64_exp_handler\num:
    cli
    SAVE_REGS

    mov  rdi, \num
    call x64_handle_exception

    RESTORE_REGS
    iretq
.endm

.extern x64_handle_unkown_irq
.globl x64_unknown_irq_handler
.align 8
x64_unknown_irq_handler:
    cli
    SAVE_REGS

    call x64_handle_unkown_irq

    RESTORE_REGS
    iretq

EXP_HANDLER_WITHOUT_ERRCODE 0
EXP_HANDLER_WITHOUT_ERRCODE 1
EXP_HANDLER_WITHOUT_ERRCODE 2
EXP_HANDLER_WITHOUT_ERRCODE 3
EXP_HANDLER_WITHOUT_ERRCODE 4
EXP_HANDLER_WITHOUT_ERRCODE 5
EXP_HANDLER_WITHOUT_ERRCODE 6
EXP_HANDLER_WITHOUT_ERRCODE 7
EXP_HANDLER_WITH_ERRCODE    8
EXP_HANDLER_WITHOUT_ERRCODE 9
EXP_HANDLER_WITH_ERRCODE    10
EXP_HANDLER_WITH_ERRCODE    11
EXP_HANDLER_WITH_ERRCODE    12
EXP_HANDLER_WITH_ERRCODE    13
EXP_HANDLER_WITH_ERRCODE    14
EXP_HANDLER_WITH_ERRCODE    15
EXP_HANDLER_WITHOUT_ERRCODE 16
EXP_HANDLER_WITH_ERRCODE    17
EXP_HANDLER_WITHOUT_ERRCODE 18
EXP_HANDLER_WITHOUT_ERRCODE 19
EXP_HANDLER_WITHOUT_ERRCODE 20

INTR_HANDLER 0x20
INTR_HANDLER 0x21
INTR_HANDLER 0x22
INTR_HANDLER 0x23
INTR_HANDLER 0x24
INTR_HANDLER 0x25
INTR_HANDLER 0x26
INTR_HANDLER 0x27
INTR_HANDLER 0x28
INTR_HANDLER 0x29
INTR_HANDLER 0x2a
INTR_HANDLER 0x2b
INTR_HANDLER 0x2c
INTR_HANDLER 0x2d
INTR_HANDLER 0x2e
INTR_HANDLER 0x2f
INTR_HANDLER 0x30
INTR_HANDLER 0x31
INTR_HANDLER 0x32
INTR_HANDLER 0x33
INTR_HANDLER 0x34
INTR_HANDLER 0x35
INTR_HANDLER 0x36
INTR_HANDLER 0x37
INTR_HANDLER 0x38
INTR_HANDLER 0x39
INTR_HANDLER 0x3a
INTR_HANDLER 0x3b
INTR_HANDLER 0x3c
INTR_HANDLER 0x3d
INTR_HANDLER 0x3e
INTR_HANDLER 0x3f
INTR_HANDLER 0x40
