#include <resea/types.h>
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "apic.h"
#include "paging.h"
#include "serial.h"

extern uint8_t __bss_start;
extern uint8_t __bss_end;

static inline void clear_bss_section(void) {
    /* Clear .bss section. */
    for (uint8_t *p = &__bss_start; p < &__bss_end; p++) {
        *p = 0;
    }
}

void kernel_init(void);

void x64_init(void) {
    static bool initialized = false;

    // Note that the kernel memory allocator is not initialized yet.
    if (!initialized) {
        clear_bss_section();
        x64_init_serial();
    }

    initialized = true;
    kernel_init();
}


void arch_init(void) {
    // Now we are able to use kernel memory allocator.

    // Initialize paging table first: we need mappings to
    // Local APIC.
    x64_init_paging();

    // Local APIC have to be initialized *just* after page
    // table initialization because CPUVAR uses Local APIC
    // internally.
    x64_init_apic();

    x64_init_gdt();
    x64_init_tss();
    x64_init_idt();
}
