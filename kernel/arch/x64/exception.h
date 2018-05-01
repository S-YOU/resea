#ifndef __X64_EXCEPTION_H__
#define __X64_EXCEPTION_H__

#include <kernel/types.h>

#define EXP_PAGE_FAULT 14

void x64_handle_exception(uint8_t exception, uint64_t error);

#endif
