#include <resea/types.h>
#include <kernel/memory.h>
#include <kernel/thread.h>
#include "thread.h"
#include "switch.h"
#include "asm.h"
#include "msr.h"


void arch_create_thread(struct arch_thread *arch, bool is_kernel_thread,
                    uintptr_t start, uintmax_t arg,
                    uintptr_t stack, size_t stack_size) {

    if (is_kernel_thread) {
        // Temporarily use the stack to pass `start` and `arg`
        // to start_kernel_thread.
        uint64_t *rsp = (uint64_t *) (stack + stack_size);
        rsp -= 2;
        rsp[0] = arg;
        rsp[1] = start;

        arch->rip = (uint64_t) start_kernel_thread;
        arch->rsp = (uint64_t) rsp;
        arch->rflags = KERNEL_DEFAULT_RFLAGS;
        arch->gs = KERNEL_DS;
        arch->is_user = false;
    } else {
        arch->gsinfo.kstack = (uintptr_t) kmalloc(KERNEL_STACK_SIZE, KMALLOC_NORMAL);
        arch->gsinfo.rsp0 = arch->gsinfo.kstack + KERNEL_STACK_SIZE;

        // Temporarily use the kernel stack to pass `arg` and an IRET frame
        // to enter_userspace.
        uint64_t *rsp0 = (uint64_t *) arch->gsinfo.rsp0;
        rsp0 -= 6;
        rsp0[0] = arg;
        rsp0[1] = start;
        rsp0[2] = USER_CS | USER_RPL;
        rsp0[3] = USER_DEFAULT_RFLAGS;
        rsp0[4] = stack + stack_size;
        rsp0[5] = USER_DS | USER_RPL;

        arch->rip = (uint64_t) enter_userspace;
        arch->rsp = (uint64_t) rsp0;
        arch->rflags = ENTER_USERSPACE_DEFAULT_RFLAGS;
        arch->gs = KERNEL_DS;
        arch->is_user = true;
    }
}

void arch_destroy_thread(struct arch_thread *arch) {
    if (arch->is_user) {
        kfree((void *) arch->gsinfo.kstack);
    }
}
