#ifndef __X64_TSS_H__
#define __X64_TSS_H__

struct tss {
    uint32_t  reserved0;
    uint64_t  rsp0;
    uint64_t  rsp1;
    uint64_t  rsp2;
    uint64_t  reserved1;
    uint64_t  ist[7];
    uint64_t  reserved2;
    uint16_t  reserved3;
    uint16_t  iomap;
} PACKED;

#define INTR_STACK_SIZE (16 * 1024)
#define INTR_HANDLER_IST 1

void x64_init_tss(void);

#endif
