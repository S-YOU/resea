#include <string.h>
#include <kernel/types.h>
#include "thread.h"
#include "process.h"
#include "memory.h"
#include "cpu.h" // FIXME

paddr_t allocated;
int used = 0;
paddr_t alloc_pages(size_t size, UNUSED int flags) {
    size = ROUND_UP(size, PAGE_SIZE);
    paddr_t addr = allocated;
    allocated += size;

    // XXX: The page could be not mapped.
    memset(from_paddr(addr), 0, size);

    used += size;
//    DEBUG("kernel: allocated %d bytes at %p (%dKB used)",
//        size, addr, used / 1024);

    return addr;
}


void *kmalloc(size_t size, int flags) {
    return from_paddr(alloc_pages(size, flags));
}


void kfree(UNUSED void *ptr) {
}

void add_vmarea(
    struct vmspace *vms,
    uintptr_t address,
    size_t length,
    int flags,
    paddr_t (*pager)(void *arg, uintptr_t addr, size_t length),
    void *pager_arg
) {
    struct vmarea *area = kmalloc(sizeof(*area), KMALLOC_NORMAL);
    area->offset = address;
    area->length = length;
    area->flags = flags;
    area->pager = pager;
    area->pager_arg = pager_arg;
    vmarea_list_append(&vms->vma, area);
}


void memory_create_vmspace(struct vmspace *vms) {

    vmarea_list_init(&vms->vma);
    arch_create_vmspace(&vms->arch);
}


void memory_destroy_vmspace(UNUSED struct vmspace *vms) {

    PANIC("%s: not yet implemented", __func__);
}


void handle_page_fault(uintptr_t address, bool invalid, bool user, bool write, UNUSED bool exec) {
    address = ROUND_DOWN(address, PAGE_SIZE);

    if (invalid) {
        goto invalid_access;
    }

    if (!user) {
        PANIC("page fault in kernel: %p", address);
    }

    struct vmspace *vms = &CPUVAR->current_thread->process->vms;
    for (struct vmarea *area = vms->vma; area != NULL; area = area->next) {
        if (area->offset <= address && address < area->offset + area->length) {
            int requested = 0;
            requested |= user ? PAGE_USER : 0;
            requested |= write ? PAGE_WRITABLE : 0;
            // TODO: NX-bit

            // Check the access right.
            if ((area->flags & requested) != requested) {
                goto invalid_access;
            }

            // A valid page access. Fill and link the page.
            paddr_t paddr = area->pager(area->pager_arg, address, PAGE_SIZE);
            INFO("Filling %p -> %p",paddr, address);
            if (paddr == 0) {
                INFO("page fault: pager error");
                thread_destroy_current();
            }

            arch_link_page(&vms->arch, address, paddr, 1, requested);
            return;
        }
    }

invalid_access:
    INFO("page fault: invalid page access: %p, address");
    thread_destroy_current();
}


void memory_init(void) {
    allocated = 0x001000000;
}
