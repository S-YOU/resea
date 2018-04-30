#include <resea/types.h>
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "apic.h"
#include "paging.h"
#include "serial.h"
#include "smp.h"
#include "syscall.h"

#define X64_THREAD_TESTER
#ifdef X64_THREAD_TESTER
#include <kernel/process.h>
#include <kernel/thread.h>

void thread_a(void) {
    while (1) {
        arch_putchar('A');
        arch_putchar('\n');
        for (volatile int i = 0x100000; i > 0; i--);
    }
}
void thread_b(void) {
    while (1) {
        arch_putchar('B');
        arch_putchar('\n');
        for (volatile int i = 0x100000; i > 0; i--);
    }
}
void thread_c(void) {
    while (1) {
        arch_putchar('C');
        arch_putchar('\n');
        for (volatile int i = 0x100000; i > 0; i--);
    }
}

void bin_container(void) {
    INLINE_ASM(
        ".globl __bin_start\n"
        "__bin_start:\n"
        "1:\n"
        "   xchgw %bx,%bx\n"
        "   movq $0xabcdef, %rax\n"
        "   syscall\n"
        "   jmp 1b\n"
        ".globl __bin_end\n"
        "__bin_end:\n"
    );
}

#include <string.h>
extern char __bin_start;
extern char __bin_end;
paddr_t bin_pager(UNUSED void *arg, uintptr_t addr, size_t length) {
    paddr_t paddr = alloc_pages(length, KMALLOC_NORMAL);
    void *ptr = from_paddr(paddr);
    memcpy(ptr, &__bin_start, (uintptr_t) &__bin_end - (uintptr_t) &__bin_start);
    return paddr;

}


void thread_tester(void) {
    INFO("x64: starting thread tester");
    struct thread *t_a = thread_create(kernel_process, (uintptr_t) thread_a, 0);
    struct thread *t_b = thread_create(kernel_process, (uintptr_t) thread_b, 0);
    struct thread *t_c = thread_create(kernel_process, (uintptr_t) thread_c, 0);
    thread_set_state(t_a, THREAD_RUNNABLE);
    thread_set_state(t_b, THREAD_RUNNABLE);
    thread_set_state(t_c, THREAD_RUNNABLE);

    struct process *user_process = process_create();
    uintptr_t bin_addr = 0x01000000;
    add_vmarea(&user_process->vms, bin_addr, PAGE_SIZE, PAGE_USER,
        bin_pager, NULL);
    struct thread *t_d = thread_create(user_process, bin_addr, 0);
    struct thread *t_e = thread_create(user_process, bin_addr, 0);
    thread_set_state(t_d, THREAD_RUNNABLE);
    thread_set_state(t_e, THREAD_RUNNABLE);
}
#endif

extern uint8_t __bss_start;
extern uint8_t __bss_end;

static inline void clear_bss_section(void) {
    /* Clear .bss section. */
    for (uint8_t *p = &__bss_start; p < &__bss_end; p++) {
        *p = 0;
    }
}

void kernel_init(void);

void x64_init(void) {
    static bool initialized = false;

    // Note that the kernel memory allocator is not initialized yet.
    if (!initialized) {
        clear_bss_section();
        x64_init_serial();
        x64_init_pic();
    }

    initialized = true;
    kernel_init();
}


void arch_early_init(void) {
    // Now we are able to use kernel memory allocator.

    // Initialize paging table first: we need mappings to
    // Local APIC.
    x64_init_paging();

    // Local APIC have to be initialized *just* after page
    // table initialization because CPUVAR uses Local APIC
    // internally.
    x64_init_apic();

    x64_init_gdt();
    x64_init_tss();
    x64_init_idt();

    x64_init_smp();
    x64_init_apic_timer();
    x64_init_syscall();
}


void arch_init(void) {
    // All kernel components are initialized.

#ifdef X64_THREAD_TESTER
    thread_tester();
#endif
}
