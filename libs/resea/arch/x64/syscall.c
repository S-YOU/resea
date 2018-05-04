#include <resea.h>

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
        "movabs $0xabcdef0123456789, %r15\n" \
    );
#define STUB_END __asm__ __volatile__("xchg %bx,%bx; nop");

channel_t ipc_open(void) {
    channel_t ch;

    __asm__ __volatile__(
        "mov $1, %%r14  \n"
        "syscall"
    : "=a"(ch)
    :
    : "%r12", "%r14", "%r15");

    return ch;
}


type_t ipc_send(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3
){
    type_t ret;

    /* TODO */
    return ret;
}

type_t ipc_recv(
    channel_t ch,
    channel_t *from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
){
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


type_t ipc_call(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3,
    payload_t *r0,
    payload_t *r1,
    payload_t *r2,
    payload_t *r3
){
    type_t ret;

    STUB_END
    __asm__ __volatile__(
        "mov $5, %%r14  \n"
        "mov %5, %%rdi  \n"
        "mov %6, %%rsi  \n"
        "mov %7, %%rdx  \n"
        "mov %8, %%rbx  \n"
        "mov %9, %%r8   \n"
        "mov %10, %%r9  \n"
        "syscall        \n"
        "mov %%r8, %1   \n"
        "mov %%r9, %2   \n"
        "mov %%r10, %3  \n"
        "mov %%r12, %4  \n"
    : "=a"(ret), "=m"(*r0), "=m"(*r1), "=m"(*r2), "=m"(*r3)
    : "r"(ch), "r"(type), "r"(a0), "r"(a1), "r"(a2), "r"(a3)
    : "%r14");
    STUB_END

    return ret;
}


type_t ipc_replyrecv(
    channel_t server,
    channel_t *client,
    type_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
){
    channel_t reply_to = *client;
    type_t ret;

    __asm__ __volatile__(
        "mov $6, %%r14  \n"
        "mov %6, %%rdi  \n"
        "mov %7, %%rsi  \n"
        "mov %8, %%rdx  \n"
        "mov %9, %%rbx  \n"
        "mov %10, %%r8  \n"
        "mov %11, %%r9  \n"
        "mov %12, %%r10 \n"
        "syscall        \n"
        "mov %%r8, %1   \n"
        "mov %%r9, %2   \n"
        "mov %%r10, %3  \n"
        "mov %%r12, %4  \n"
        "mov %%r13, %5  \n"
    : "=a"(ret), "=m"(*client), "=m"(*a0), "=m"(*a1), "=m"(*a2), "=m"(*a3)
    : "r"(server), "r"(reply_to), "r"(type), "r"(r0), "r"(r1), "r"(r2), "r"(r3)
    : "%r14");

    return ret;
}


type_t ipc_discard(payload_t ool0, payload_t ool1, payload_t ool2, payload_t ool3){
    /* TODO */
}


type_t ipc_link(channel_t ch0, channel_t ch1){
    /* TODO */
}


type_t ipc_transfer(channel_t ch, channel_t dest){
    /* TODO */
}


channel_t ipc_connect(channel_t server){
    /* TODO */
}
