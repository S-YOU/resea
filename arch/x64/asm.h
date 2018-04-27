#ifndef __X64_ASM_H__
#define __X64_ASM_H__

#include <resea/types.h>

static inline void asm_outb(uint16_t port, uint8_t value) {
    INLINE_ASM("outb %0, %1" :: "a"(value), "Nd"(port));
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

static inline void asm_wrmsr(uint32_t reg, uint64_t value) {
    INLINE_ASM("wrmsr" :: "c"(reg), "A"(value));
}

static inline uint64_t asm_rdmsr(uint32_t reg) {
    uint64_t value;
    INLINE_ASM("rdmsr" : "=A"(value) : "c"(reg));
    return value;
}

static inline void asm_invlpg(uint64_t vaddr) {
    // Specify "memory" in clobber list to prevent memory
    // access reordering.
    INLINE_ASM("invlpg (%0)" :: "b"(vaddr) : "memory");
}

#endif
