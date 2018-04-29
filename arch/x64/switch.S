/* TODO: rewrite in Intel syntax */
#define ATT_SYNTAX
#include "asmdefs.h"
#include "msr.h"

/* void arch_switch(struct arch_thread *prev, struct arch_thread *next); */
.globl arch_switch
arch_switch:
    /* set GS base addresses. Note that we're in kernel space; the
       current GS base will be swapped by SWAPGS in the interrupt
       handler. */
    mov 40(%rsi), %rdx
    mov $MSR_GS_BASE, %ecx
    mov %edx, %eax
    shrq $32, %rdx
    wrmsr

    /* We don't allow using GS register in userspace for now. */
    mov $MSR_KERNEL_GS_BASE, %ecx
    xor %eax, %eax
    xor %edx, %edx
    wrmsr

    movabs $1f, %rax
    movq %rax, 0(%rdi)  /* Set prev RIP to label "1". */
    movq %rsp, 24(%rdi)  /* Save the current RSP to prev. */
    movq 24(%rsi), %rsp  /* Restore next's RSP. */
    sti                 /* XXX: accept interrupts. */
    jmpq *0(%rsi)       /* Resume next. */

1:
    ret
