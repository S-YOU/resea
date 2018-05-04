.intel_syntax noprefix

#define STUB \
    __asm__ __volatile__( \
        "movabs $0xabcdef0123456789, %rax\n" \
        "movabs $0xabcdef0123456789, %rbx\n" \
        "movabs $0xabcdef0123456789, %rcx\n" \
        "movabs $0xabcdef0123456789, %rdx\n" \
        "movabs $0xabcdef0123456789, %rdi\n" \
        "movabs $0xabcdef0123456789, %rsi\n" \
        "movabs $0xabcdef0123456789, %rbp\n" \
        "movabs $0xabcdef0123456789, %r8\n" \
        "movabs $0xabcdef0123456789, %r9\n" \
        "movabs $0xabcdef0123456789, %r10\n" \
        "movabs $0xabcdef0123456789, %r11\n" \
        "movabs $0xabcdef0123456789, %r12\n" \
        "movabs $0xabcdef0123456789, %r13\n" \
        "movabs $0xabcdef0123456789, %r14\n" \
        "movabs $0xabcdef0123456789, %r13\n" \
    );
#define STUB_END __asm__ __volatile__("xchg %bx,%bx; nop");

/* channel_t ipc_open(void) */
.globl ipc_open
ipc_open:
    push r13
    push r14
    push r12

    mov r14, 1
    syscall

    pop r12
    pop r14
    pop r13
    ret


/*
 *  type_t ipc_send(
 *      channel_t ch,
 *      type_t type,
 *      payload_t a0,
 *      payload_t a1,
 *      payload_t a2,
 *      payload_t a3
 *  );
 */
.globl ipc_send
ipc_send:
    ret

/*
 *  type_t ipc_recv(
 *      channel_t ch,
 *      channel_t *from,
 *      payload_t *a0,
 *      payload_t *a1,
 *      payload_t *a2,
 *      payload_t *a3
 *  );
 */
 .globl ipc_recv
ipc_recv:
    ret
/*
       type_t ret;

    STUB
    __asm__ __volatile__(
        "mov $4, %%r14  \n"
        "mov %6, %%rdi  \n"
        "syscall        \n"
        "mov %%r8, %1   \n"
        "mov %%r9, %2   \n"
        "mov %%r10, %3  \n"
        "mov %%r12, %4  \n"
        "mov %%r13, %5  \n"
    : "=a"(ret), "=m"(*from), "=m"(*a0), "=m"(*a1), "=m"(*a2), "=m"(*a3)
    : "r"(ch)
    : "%r14");
    STUB_END

    return ret;
}
*/

/*
 *  type_t ipc_call(
 *      channel_t ch,
 *      type_t type,
 *      payload_t a0,
 *      payload_t a1,
 *      payload_t a2,
 *      payload_t a3,
 *      payload_t *r0,
 *      payload_t *r1,
 *      payload_t *r2,
 *      payload_t *r3
 *  );
 */
.globl ipc_call
ipc_call:
    push rbp
    mov rbp, rsp
    push r13
    push r14
    push r12

    mov r14, 5
    syscall

    mov r8, [rbp + 16]
    mov r9, [rbp + 24]
    mov r10, [rbp + 32]
    mov r12, [rbp + 40]

    pop r12
    pop r14
    pop r13
    pop rbp
    ret

/*
 *  type_t ipc_replyrecv(
 *      channel_t server,
 *      channel_t *client,
 *      type_t type,
 *      payload_t r0,
 *      payload_t r1,
 *      payload_t r2,
 *      payload_t r3,
 *      payload_t *a0,
 *      payload_t *a1,
 *      payload_t *a2,
 *      payload_t *a3
 *
 */
 .globl ipc_replyrecv
ipc_replyrecv:
    push rbp
    mov rbp, rsp
    push r13
    push r14
    push r12
    push rsi

    mov r14, 6
    syscall

    pop rsi
    mov r8, [rsi]
    mov r9, [rbp + 16]
    mov r10, [rbp + 24]
    mov r12, [rbp + 32]
    mov r13, [rbp + 40]

    pop r12
    pop r14
    pop r13
    pop rbp
    ret

/* type_t ipc_discard(payload_t ool0, payload_t ool1, payload_t ool2, payload_t ool3); */
.globl ipc_discard
ipc_discard:
    ret

/* type_t ipc_link(channel_t ch0, channel_t ch1); */
.globl ipc_link
ipc_link:
    ret

/* type_t ipc_transfer(channel_t ch, channel_t dest); */
.globl ipc_transfer
ipc_transfer:
    ret

/* channel_t ipc_connect(channel_t server); */
.globl ipc_connect
ipc_connect:
    push r13
    push r14
    push r12

    mov r14, 9
    syscall

    pop r12
    pop r14
    pop r13
    ret
