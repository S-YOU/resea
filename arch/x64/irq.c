#include <resea/types.h>
#include <printf.h>
#include "irq.h"

void x64_handle_unkown_irq(void) {
    printf("Unknown Interrupt\n");
}


void x64_handle_irq(uint8_t vector) {
    printf("Interrupt %d\n", vector);
}
