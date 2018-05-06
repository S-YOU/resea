#include <kernel/types.h>
#include "asm.h"


void arch_idle(void) {
    asm_stihlt();
}


void arch_panic(void) {
    asm_cli();
    asm_hlt();
}