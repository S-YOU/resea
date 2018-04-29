#ifndef __X64_USER_H__
#define __X64_USER_H__

#include "gdt.h"

#define USER_CS   (GDT_USER_CODE * 8)
#define USER_DS   (GDT_USER_DATA * 8)
#define USER_RPL  3

// IF and reserved bit set.
#define USER_DEFAULT_RFLAGS (0x202)

#endif
