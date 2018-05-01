#ifndef __X64_IDT_H__
#define __X64_IDT_H__

#include <resea/types.h>

#define IDT_DESC_NUM    256
#define IDT_LENGTH      ((IDT_DESC_NUM * sizeof(struct intr_desc)) + 1)
#define IDT_INT_HANDLER 0x8e

struct intr_desc {
    uint16_t offset1;
    uint16_t seg;
    uint8_t  ist;
    uint8_t  info;
    uint16_t offset2;
    uint32_t offset3;
    uint32_t reserved;
} PACKED;

struct idtr {
    uint16_t length;
    uint64_t address;
} PACKED;

void x64_init_idt(void);
void x64_set_intr_desc(struct intr_desc *desc, uint8_t ist, uint16_t seg, paddr_t offset);

#endif
