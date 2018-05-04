#ifndef __ARCH_TYPES_H__
#define __ARCH_TYPES_H__

// For debugging on Bochs.
#define MAGICBREAK __asm__ __volatile__("xchgw %bx,%bx");
#define KERNEL_BASE_ADDR 0xffff800000000000
#define PAGE_SIZE 4096
#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)

typedef unsigned long long paddr_t;
typedef unsigned long long uintptr_t;
typedef unsigned long long uintmax_t;

/* TODO: lock-free! */
#define KMUTEX_UNLOCKED 0
#define KMUTEX_LOCKED 1
typedef uint32_t kmutex_t;
typedef uint64_t kmutex_state_t;

static inline void kmutex_init(kmutex_t *lock, int init) {
    *lock = init;
}

static inline kmutex_state_t kmutex_lock_irq_disabled(kmutex_t *lock) {
    kmutex_state_t state;

    INLINE_ASM(
        "1:                        \n"
        "    pause                 \n"
        "    lock cmpxchgl %1, %2  \n"
        "    jnz 1b                \n"
        "    pushfq                \n "
        "    pop %%rax             \n"
    : "=a"(state)
    : "r"(KMUTEX_UNLOCKED), "m"(lock)
    );

    return state;
}

static inline void kmutex_unlock_restore_irq(kmutex_t *lock, kmutex_state_t state) {
    INLINE_ASM(
        "push $2      \n"
        "popfq        \n"
        "movl %1, %0  \n"
    : "=m"(lock)
    : "r"(KMUTEX_UNLOCKED), "r"(state)
    );
}

static inline void *from_paddr(paddr_t addr) {
    return (void *) (addr | KERNEL_BASE_ADDR);
}

static inline paddr_t to_paddr(void *addr) {
    return ((uintptr_t) addr & ~KERNEL_BASE_ADDR);
}

// Don't forget to update hardcoded offsets in switch.S and
// handler.S!
struct gsinfo {
    // A pointer to the dedicated kernel stack. This value
    // is used by only userspace threads.
    uint64_t rsp0; // kstack + sizeof(kstack)
    uint64_t kstack;
};

// Don't forget to update hardcoded offsets in switch.S!
struct arch_thread {
    // IRET frame.
    uint64_t rip;          // offset: 0
    uint64_t rsp;          // offset: 8
    uint64_t rflags;       // offset: 16
    uint64_t is_user;      // offset: 24
    uint64_t gs;           // offset: 32
    struct gsinfo gsinfo;  // offset: 40
};

struct arch_vmspace {
    paddr_t pml4_addr;
};

#endif
