#include <kernel/types.h>
#include "apic.h"
#include "irq.h"
#include <kernel/thread.h>

void x64_handle_unkown_irq(void) {
    x64_ack_interrupt();
    INFO("Unknown Interrupt");
}


void x64_handle_irq(UNUSED u8_t vector) {
    x64_ack_interrupt();
    // XXX
    static int tick = 0;
    // The interval MUST be long enough; short interval leads to
    // recursive timer interrupt handling, kernel stack starvation,
    // and eventually a nasty bug.
    if (tick++ > 100) {
        tick = 0;
        thread_switch();
    }
}
