#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <resea/types.h>

#define KMALLOC_ZEROED 1
#define KMALLOC_PANIC_ON_FAIL 2
#define KMALLOC_NORMAL (KMALLOC_ZEROED | KMALLOC_PANIC_ON_FAIL)

struct vmspace {
    struct arch_vmspace arch;
};

paddr_t alloc_pages(size_t size, int flags);
void *kmalloc(size_t size, int flags);
void memory_init(void);

#endif
