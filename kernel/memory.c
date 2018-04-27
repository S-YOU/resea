#include <string.h>
#include "memory.h"

paddr_t allocated;
paddr_t alloc_pages(size_t size, int flags) {
    size = ROUND_UP(size, PAGE_SIZE);
    paddr_t addr = allocated;
    allocated += size;

    // XXX: The page could be not mapped.
    memset(from_paddr(addr), 0, size);

    return addr;
}


void *kmalloc(size_t size, int flags) {
    return from_paddr(alloc_pages(size, flags));
}


void memory_init(void) {
    allocated = 0x001000000;
}
