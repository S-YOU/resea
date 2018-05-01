#ifndef __X64_ASM_H__
#define __X64_ASM_H__

#include <kernel/types.h>

static inline void asm_outb(uint16_t port, uint8_t value) {
    INLINE_ASM("outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline uint8_t asm_inb(uint16_t port) {
    uint8_t value;

    INLINE_ASM("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Don't divide into asm_sti and asm_hlt!
static inline void asm_stihlt(void) {
    INLINE_ASM("sti; hlt");
}

static inline void asm_lgdt(uintptr_t gdtr) {
    INLINE_ASM("lgdt (%%rax)" :: "a"(gdtr));
}

static inline void asm_lidt(uintptr_t idtr) {
    INLINE_ASM("lidt (%%rax)" :: "a"(idtr));
}

static inline void asm_ltr(uint16_t tr) {
    INLINE_ASM("ltr %0" :: "a"(tr));
}

static inline void asm_set_cr3(uint64_t value) {
    INLINE_ASM("mov %0, %%cr3" :: "r"(value) : "memory");
}

static inline uint64_t asm_read_cr2(void) {
    uint64_t value;
    INLINE_ASM("mov %%cr2, %0" : "=r"(value));
    return value;
}

static inline void asm_wrmsr(uint32_t reg, uint64_t value) {
    uint32_t low = value & 0xffffffff;
    uint32_t hi = value >> 32;
    INLINE_ASM("wrmsr" :: "c"(reg), "a"(low), "d"(hi));
}

static inline uint64_t asm_rdmsr(uint32_t reg) {
    uint32_t low, high;
    INLINE_ASM("rdmsr" : "=a"(low), "=d"(high) : "c"(reg));
    return ((uint64_t) high << 32) | low;
}

static inline void asm_invlpg(uint64_t vaddr) {
    // Specify "memory" in clobber list to prevent memory
    // access reordering.
    INLINE_ASM("invlpg (%0)" :: "b"(vaddr) : "memory");
}

#endif
