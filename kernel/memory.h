#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <kernel/types.h>
#include <list.h>

#define KMALLOC_ZEROED 1
#define KMALLOC_PANIC_ON_FAIL 2
#define KMALLOC_NORMAL (KMALLOC_ZEROED | KMALLOC_PANIC_ON_FAIL)

struct vmspace;
struct vmarea {
    struct vmarea *next;
    uintptr_t offset;
    size_t length;
    int flags;
    paddr_t (*pager)(void *arg, uintptr_t addr, size_t length);
    void *pager_arg;
};

DEFINE_LIST(vmarea, struct vmarea)

struct vmspace {
    struct arch_vmspace arch;
    struct vmarea *vma;
};


paddr_t alloc_pages(size_t size, int flags);
void *kmalloc(size_t size, int flags);
void kfree(void *ptr);
void add_vmarea(
    struct vmspace *vms,
    uintptr_t address,
    size_t length,
    int flags,
    paddr_t (*pager)(void *arg, uintptr_t addr, size_t length),
    void *pager_arg
);
void memory_create_vmspace(struct vmspace *vms);
void memory_destroy_mspace(struct vmspace *vms);
void handle_page_fault(uintptr_t address, bool invalid, bool user, bool write, bool exec);
void memory_init(void);

#endif
