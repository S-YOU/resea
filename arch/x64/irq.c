#include <resea/types.h>
#include "apic.h"
#include "irq.h"
#include <kernel/thread.h>

void x64_handle_unkown_irq(void) {
    x64_ack_interrupt();
    INFO("Unknown Interrupt");
}


void x64_handle_irq(UNUSED uint8_t vector) {
    x64_ack_interrupt();

    // XXX
    static int tick = 0;
    if (tick++ > 10) {
        thread_switch();
    }
}
