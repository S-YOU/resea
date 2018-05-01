#ifndef __ELF_H__
#define __ELF_H__

#include "types.h"
#include "memory.h"

void elf_create_process(const void *image, size_t length, pager_t *pager, void *pager_arg);

#endif
