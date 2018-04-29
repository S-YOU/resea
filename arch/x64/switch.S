.intel_syntax noprefix

/* void arch_switch(struct arch_thread *prev, struct arch_thread *next); */
.globl arch_switch
arch_switch:
    movabs rax, 1f
    mov [rdi + 0], rax  /* Set prev RIP to label "1". */
    mov [rdi + 8], rsp  /* Save the current RSP to prev. */
    mov rsp, [rsi + 8]  /* Restore next's RSP. */
    sti                 /* Accept interrupts. */
    xchg bx,bx
    jmp [rsi + 0]       /* Resume next. */

1:
    xchg bx,bx
    ret
