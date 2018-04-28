#include <resea/types.h>

void arch_init_arch(struct arch_thread *arch, uintptr_t start, uintptr_t stack, size_t stack_size) {
    arch->rip = start;
    arch->rsp = stack + stack_size;
}

void returned_from_thread(void) {
    // Just return to resume the thread.
}


void arch_switch(struct arch_thread *prev, struct arch_thread *next) {
    INLINE_ASM(
        "   movq %4, %0\n"    // Set prev RIP to label "1".
        "   movq %%rsp, %1\n" // Save the current RSP to prev.
        "   movq %3, %%rsp\n" // Restore next's RSP.
        "   pushq %2\n"       // Push next's RIP to RET.
        "   ret\n"            // Resume the next.

        "1:"                 // Threads resume from here.
        "   ret\n"           // Return from arch_switch.
    : "=m"(prev->rip), "=m"(prev->rsp)
    : "r"(next->rip), "r"(next->rsp), "r"(returned_from_thread)
    );
}


void arch_first_switch(struct arch_thread *next) {
    INLINE_ASM(
        "   movq %1, %%rsp\n" // Restore next's RSP.
        "   pushq %0\n"       // Push next's RIP to RET.
        "   ret\n"            // Resume the next.
    :: "r"(next->rip), "r"(next->rsp));
}
