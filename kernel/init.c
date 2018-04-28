#include "memory.h"
#include "process.h"
#include "thread.h"

void thread_a() {
    while (1) {
        arch_putchar('A');
    }
}

void thread_b() {
    while (1) {
        arch_putchar('B');
    }
}

void thread_c() {
    while (1) {
        arch_putchar('C');
    }
}


void kernel_init(void) {
    INFO("Starting Resea...");

    memory_init();
    arch_init();
    process_init();
    thread_init();

    struct process *kernel = process_create();
    struct thread *t_a = thread_create(kernel, (uintptr_t) thread_b, 0);
    struct thread *t_b = thread_create(kernel, (uintptr_t) thread_a, 0);
    struct thread *t_c = thread_create(kernel, (uintptr_t) thread_c, 0);
    thread_set_state(t_a, THREAD_RUNNABLE);
    thread_set_state(t_b, THREAD_RUNNABLE);
    thread_set_state(t_c, THREAD_RUNNABLE);

    INFO("Kernel initialized.");

    MAGICBREAK
    *((char *) 0x1234789) = 0x33;

    for(;;);

    for (;;) {
        thread_switch();
        INFO("Returned from thread_switch().\n");
    }
}
