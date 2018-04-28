#include <resea/types.h>
#include "irq.h"

void x64_handle_unkown_irq(void) {
    INFO("Unknown Interrupt");
}


void x64_handle_irq(uint8_t vector) {
    INFO("Interrupt %d", vector);
}
