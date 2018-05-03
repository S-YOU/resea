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
     *  RDI: 1st argument
     *  RSI: 2nd argument
     *  RDX: 3rd argument
     *  RCX: user RIP
     *  R8:  5th arugment
     *  R9:  6th arugment
     *  R10: 7th arugment
     *  R11: user RFLAGS
     *  R12: 4th argument
     *  R14: syscall number
     *  R15: user RSP
     *
     */

    /* Switch to the kernel stack. */
    swapgs
    mov r15, rsp
    mov rsp, gs:[0]

    /* TODO: save callee saved registers  */

    push r11 /* user RFLAGS */
    push r15 /* user RSP */
    push rcx /* user RIP */
    xchg rcx, rbx

    call handle_syscall
    /*
     * It returns these registers:
     *
     * RAX: type
     * R8: 1st return value
     * R9: 2nd return value
     * R10: 3rd return value
     * R11: (unused)
     * R12: 4th return value
     * R13: 5th return value
     *
     */

    /* Sanitize callee-saved registers not to leak information to user. */
    xor rdi, rdi
    xor rsi, rsi
    xor rdx, rdx

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
    mov rsi, rsp

    /* Check CS register in the IRET frame. */
    test qword ptr [rsi + 16], 3
    jz 1f

    /* from user: use dedicated kernel stack */
    swapgs

    mov rsp, gs:[0] /* Extract RSP from gsinfo. */
    jmp 2f
1:
    /* from kernel: use current stack (assuming Red Zone is disabled) */
    mov rsp, [rsp + 32] /* Extract RSP from the IRET frame */
    jmp 2f

2:
    /* Copy IRET frame. */
    push [rsi + 40]    /* SS */
    push [rsi + 32]    /* RSP */
    push [rsi + 24]    /* RFLAGS */
    push [rsi + 16]    /* CS */
    push [rsi + 8]     /* RIP */
    mov rsi, [rsi + 0] /* Restore RSI. */
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
    xchg bx,bx
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
