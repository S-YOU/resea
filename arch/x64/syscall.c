#include "syscall.h"
#include "gdt.h"
#include "msr.h"
#include "asm.h"
#include "handler.h"

void x64_init_syscall(void) {
    uint64_t star = ((uint64_t) USER_CODE64_SEG << 48) | ( (uint64_t) KERNEL_CODE64_SEG << 32);
    asm_wrmsr(MSR_STAR, star);
    asm_wrmsr(MSR_LSTAR, (uintptr_t) x64_syscall_handler);

    // RIP for compatibility mode. We don't support it for now.
    asm_wrmsr(MSR_CSTAR, 0);
    asm_wrmsr(MSR_SFMASK, 0);

    // Enable SYSCALL/SYSRET.
    asm_wrmsr(MSR_EFER, asm_rdmsr(MSR_EFER) | EFER_SCE);
}
