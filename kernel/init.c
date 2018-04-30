#include "memory.h"
#include "process.h"
#include "thread.h"


void kernel_init(void) {
    INFO("Starting Resea...");
    INFO("kernel: initializing memory system");
    memory_init();
    arch_early_init();
    INFO("kernel: initializing process system");
    process_init();
    INFO("kernel: initializing thread system");
    thread_init();
    arch_init();
    INFO("kernel: initialized the kernel");

    if (thread_list_is_empty(&kernel_process->threads)) {
        PANIC("No threads to run.");
    }

    thread_switch();

    // From here, we're in the idle thread context. The thread
    // is resumed when there are no other threads to run.
    for (;;) {
        PANIC("idling...\n");
        // Sleep until an interrupt occurs in order not to heat up the computer
        // and to save money on electricity.
        arch_idle();
    }
}
