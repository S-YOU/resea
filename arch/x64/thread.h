#ifndef __X64_USER_H__
#define __X64_USER_H__

#include "gdt.h"

#define USER_CS   (GDT_USER_CODE * 8)
#define USER_DS   (GDT_USER_DATA * 8)
#define USER_RPL  3

// IF and reserved bit set.
#define USER_DEFAULT_RFLAGS (0x202)
#define KERNEL_DEFAULT_RFLAGS (0x202)

// Disable interrupts since the kernel stack temporarily holds `arg` and
// a IRET frame. If interrupt is enabled during enter_userspace, an
// interrupt handler may overwrite them.
#define ENTER_USERSPACE_DEFAULT_RFLAGS (0x002)

#endif
