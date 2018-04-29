#ifndef __ARCH_TYPES_H__
#define __ARCH_TYPES_H__

// For debugging on Bochs.
#define MAGICBREAK __asm__ __volatile__("xchgw %bx,%bx");
#define KERNEL_BASE_ADDR 0xffff800000000000
#define PAGE_SIZE 4096
#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITABLE (1 << 1)

typedef unsigned long long paddr_t;
typedef unsigned long long uintptr_t;
typedef unsigned long long uintmax_t;

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
    struct gsinfo gsinfo;  // offset: 32
};

struct arch_vmspace {
    paddr_t pml4_addr;
};

#endif
