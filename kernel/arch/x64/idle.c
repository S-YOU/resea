#include <kernel/types.h>
#include "asm.h"


void arch_idle(void) {
    asm_stihlt();
}
