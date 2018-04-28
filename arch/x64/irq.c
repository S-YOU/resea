#include <resea/types.h>
#include "apic.h"
#include "irq.h"

void x64_handle_unkown_irq(void) {
    x64_ack_interrupt();
    INFO("Unknown Interrupt");
}


void x64_handle_irq(uint8_t vector) {
    x64_ack_interrupt();
}
