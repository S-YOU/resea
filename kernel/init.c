#include "memory.h"
#include "process.h"
#include "thread.h"


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
