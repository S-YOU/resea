#include <string.h>
#include "cpu.h"
#include "asm.h"
#include "idt.h"
#include "tss.h"
#include "handler.h"

void x64_set_intr_desc(struct intr_desc *desc, uint8_t ist, uint16_t seg, paddr_t offset) {

    desc->offset1  = offset & 0xffff;
    desc->seg      = seg;
    desc->ist      = ist;
    desc->info     = IDT_INT_HANDLER;
    desc->offset2  = (offset >> 16) & 0xffff;
    desc->offset3  = (offset >> 32) & 0xffffffff;
    desc->reserved = 0;
}


void x64_init_idt(void) {
    struct intr_desc *idt = (struct intr_desc *) &CPUVAR->idt;

    memset(&CPUVAR->idt, 0, sizeof(CPUVAR->idt));
    memset(&CPUVAR->idtr, 0, sizeof(CPUVAR->idtr));

    // expeptions
    x64_set_intr_desc(&idt[0],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler0));
    x64_set_intr_desc(&idt[1],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler1));
    x64_set_intr_desc(&idt[2],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler2));
    x64_set_intr_desc(&idt[3],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler3));
    x64_set_intr_desc(&idt[4],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler4));
    x64_set_intr_desc(&idt[5],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler5));
    x64_set_intr_desc(&idt[6],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler6));
    x64_set_intr_desc(&idt[7],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler7));
    x64_set_intr_desc(&idt[8],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler8));
    x64_set_intr_desc(&idt[9],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler9));
    x64_set_intr_desc(&idt[10], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler10));
    x64_set_intr_desc(&idt[11], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler11));
    x64_set_intr_desc(&idt[12], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler12));
    x64_set_intr_desc(&idt[13], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler13));
    x64_set_intr_desc(&idt[14], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler14));
    x64_set_intr_desc(&idt[15], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler15));
    x64_set_intr_desc(&idt[16], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler16));
    x64_set_intr_desc(&idt[17], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler17));
    x64_set_intr_desc(&idt[18], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler18));
    x64_set_intr_desc(&idt[19], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler19));
    x64_set_intr_desc(&idt[20], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_exp_handler20));

    // Interrupts
    for (int i=32; i <= 255; i++) {
        x64_set_intr_desc(&idt[i], INTR_HANDLER_IST, KERNEL_CODE64_SEG, to_paddr(&x64_unknown_irq_handler));
    }

    // Update GDTR
    CPUVAR->idtr.length = IDT_LENGTH;
    CPUVAR->idtr.address = (uintptr_t) idt;
    asm_lidt((uintptr_t) &CPUVAR->idtr);
}
