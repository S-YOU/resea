#ifndef __X64_GDT_H__
#define __X64_GDT_H__

#include <resea/types.h>

#define GDT_NULL        0
#define GDT_KERNEL_CODE 1
#define GDT_KERNEL_DATA 2
#define GDT_USER_CODE32 3
#define GDT_USER_CODE   4
#define GDT_USER_DATA   5
#define GDT_TSS         6 // Note: a TSS descriptor is twice as large as
                          //       a code segment descriptor.
#define GDT_DESC_NUM    8

#define KERNEL_NULL_SEG    0
#define KERNEL_CODE64_SEG  (GDT_KERNEL_CODE * 8)
#define KERNEL_DATA64_SEG  (GDT_KERNEL_DATA * 8)
#define USER_DATA64_SEG    (GDT_USER_DATA * 8)
#define USER_CODE64_SEG    (GDT_USER_CODE * 8)
#define KERNEL_DATA64_SEG  (GDT_KERNEL_DATA * 8)
#define GDT_TSS_SEG        (GDT_TSS * 8)

#define GDT_LENGTH                ((sizeof(struct seg_desc) * GDT_DESC_NUM) - 1)
#define GDTTYPE_KERNEL_CODE64     0x9a
#define GDTTYPE_KERNEL_DATA64     0x92
#define GDTTYPE_USER_CODE32       0xfa
#define GDTTYPE_USER_CODE64       0xfa
#define GDTTYPE_USER_DATA64       0xf2
#define GDTTYPE_TSS               0x89
#define GDT_TSS_LIMIT             (sizeof(struct tss) - 1)
#define GDT_LIMIT2_MASK_CODE64    0xa0
#define GDT_LIMIT2_MASK_CODE32    0xc0
#define GDT_LIMIT2_MASK_DATA64    0x80

struct seg_desc {
    uint16_t limit1;
    uint16_t base1;
    uint8_t  base2;
    uint8_t  type;
    uint8_t  limit2;
    uint8_t  base3;
} PACKED;

struct tss_desc {
    uint16_t limit1;
    uint16_t base1;
    uint8_t  base2;
    uint8_t  type;
    uint8_t  limit2;
    uint8_t  base3;
    uint32_t base4;
    uint32_t reserved;
} PACKED;

struct gdtr {
    uint16_t length;
    uint64_t address;
} PACKED;

void x64_init_gdt(void);

#endif
