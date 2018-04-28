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
    arch_early_init();
    process_init();
    thread_init();
    arch_init();
    INFO("Kernel initialized.");

    thread_switch();
    PANIC("Returned from thread_switch().\n");
}
