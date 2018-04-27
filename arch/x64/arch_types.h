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

struct arch_regs {
};

struct arch_vmspace {
    paddr_t pml4_addr;
};

void arch_init_regs(struct arch_regs *regs);
void arch_switch_to(struct arch_regs *regs);

#endif
