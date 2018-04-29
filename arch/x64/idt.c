#include <string.h>
#include "cpu.h"
#include "asm.h"
#include "idt.h"
#include "tss.h"
#include "handler.h"

void x64_set_intr_desc(struct intr_desc *desc, uint8_t ist, uint16_t seg, uintptr_t offset) {

    desc->offset1  = offset & 0xffff;
    desc->seg      = seg;
    desc->ist      = ist;
    desc->info     = IDT_INT_HANDLER;
    desc->offset2  = (offset >> 16) & 0xffff;
    desc->offset3  = (offset >> 32) & 0xffffffff;
    desc->reserved = 0;
}


#define SET_IRQ_DESC(n) set_irq_desc(&idt[n], (uintptr_t) x64_irq_handler##n)
static void set_irq_desc(struct intr_desc *desc, uintptr_t offset) {

    printf("offset %p\n", offset);
    desc->offset1  = offset & 0xffff;
    desc->seg      = KERNEL_CODE64_SEG;
    desc->ist      = INTR_HANDLER_IST;
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
    x64_set_intr_desc(&idt[0],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler0);
    x64_set_intr_desc(&idt[1],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler1);
    x64_set_intr_desc(&idt[2],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler2);
    x64_set_intr_desc(&idt[3],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler3);
    x64_set_intr_desc(&idt[4],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler4);
    x64_set_intr_desc(&idt[5],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler5);
    x64_set_intr_desc(&idt[6],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler6);
    x64_set_intr_desc(&idt[7],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler7);
    x64_set_intr_desc(&idt[8],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler8);
    x64_set_intr_desc(&idt[9],  INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler9);
    x64_set_intr_desc(&idt[10], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler10);
    x64_set_intr_desc(&idt[11], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler11);
    x64_set_intr_desc(&idt[12], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler12);
    x64_set_intr_desc(&idt[13], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler13);
    x64_set_intr_desc(&idt[14], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler14);
    x64_set_intr_desc(&idt[15], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler15);
    x64_set_intr_desc(&idt[16], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler16);
    x64_set_intr_desc(&idt[17], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler17);
    x64_set_intr_desc(&idt[18], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler18);
    x64_set_intr_desc(&idt[19], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler19);
    x64_set_intr_desc(&idt[20], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) &x64_exp_handler20);

    // Interrupts
    SET_IRQ_DESC(0x20);
    SET_IRQ_DESC(0x21);
    SET_IRQ_DESC(0x22);
    SET_IRQ_DESC(0x23);
    SET_IRQ_DESC(0x24);
    SET_IRQ_DESC(0x25);
    SET_IRQ_DESC(0x26);
    SET_IRQ_DESC(0x27);
    SET_IRQ_DESC(0x28);
    SET_IRQ_DESC(0x29);
    SET_IRQ_DESC(0x2a);
    SET_IRQ_DESC(0x2b);
    SET_IRQ_DESC(0x2c);
    SET_IRQ_DESC(0x2d);
    SET_IRQ_DESC(0x2e);
    SET_IRQ_DESC(0x2f);
    SET_IRQ_DESC(0x30);
    SET_IRQ_DESC(0x31);
    SET_IRQ_DESC(0x32);
    SET_IRQ_DESC(0x33);
    SET_IRQ_DESC(0x34);
    SET_IRQ_DESC(0x35);
    SET_IRQ_DESC(0x36);
    SET_IRQ_DESC(0x37);
    SET_IRQ_DESC(0x38);
    SET_IRQ_DESC(0x39);
    SET_IRQ_DESC(0x3a);
    SET_IRQ_DESC(0x3b);
    SET_IRQ_DESC(0x3c);
    SET_IRQ_DESC(0x3d);
    SET_IRQ_DESC(0x3e);
    SET_IRQ_DESC(0x3f);
    SET_IRQ_DESC(0x40);

    for (int i=0x41; i <= 0xff; i++) {
        x64_set_intr_desc(&idt[i], INTR_HANDLER_IST, KERNEL_CODE64_SEG, (uintptr_t) x64_unknown_irq_handler);
    }

    // Update GDTR
    CPUVAR->idtr.length = IDT_LENGTH;
    CPUVAR->idtr.address = (uintptr_t) idt;
    asm_lidt((uintptr_t) &CPUVAR->idtr);
}
