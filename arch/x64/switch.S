/* TODO: rewrite in Intel syntax */

/* void arch_switch(struct arch_thread *prev, struct arch_thread *next); */
.globl arch_switch
arch_switch:
    movabs $1f, %rax
    movq %rax, 0(%rdi)  /* Set prev RIP to label "1". */
    movq %rsp, 8(%rdi)  /* Save the current RSP to prev. */
    movq 8(%rsi), %rsp  /* Restore next's RSP. */
    sti                 /* XXX: accept interrupts. */
    jmpq *0(%rsi)       /* Resume next. */

1:
    xchgw %bx,%bx
    ret
