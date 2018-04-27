#include "apic.h"
#include "asm.h"
#include "msr.h"


void x64_init_apic(void) {

    // Local APIC hardware intialization
    asm_wrmsr(MSR_IA32_APIC_BASE,
              (asm_rdmsr(MSR_IA32_APIC_BASE) & 0xfffff100) | 0x0800);

    // Local APIC software initialization
    x64_write_apic(APIC_REG_SPURIOUS_INT, 1 << 8);

    // task priority
    x64_write_apic(APIC_REG_TPR, 0);

    // logical destination register
    x64_write_apic(APIC_REG_LOGICAL_DEST, 0x01000000);

    // destination format register
    x64_write_apic(APIC_REG_DEST_FORMAT, 0xffffffff);

    // timer interrupt
    x64_write_apic(APIC_REG_LVT_TIMER, 1 << 16 /* masked */);

    // errror interrupt
    x64_write_apic(APIC_REG_LVT_ERROR, 1 << 16 /* masked */);
}
