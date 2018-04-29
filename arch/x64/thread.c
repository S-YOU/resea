#include <resea/types.h>

void arch_init_arch(struct arch_thread *arch, uintptr_t start, UNUSED uintmax_t arg,
                    uintptr_t stack, size_t stack_size) {

    arch->rip = start;
    arch->rsp = stack + stack_size;
}


NORETURN void arch_first_switch(struct arch_thread *next) {
    MAGICBREAK
    INLINE_ASM(
        "   movq %1, %%rsp\n" // Restore next's RSP.
        "   sti\n"            // Accept interrupts.
        "   jmpq *%0\n"       // Resume next.
    :: "r"(next->rip), "r"(next->rsp));

    // UNRECHABLE: add an infinite loop to suppress compiler warning.
    for (;;);
}
