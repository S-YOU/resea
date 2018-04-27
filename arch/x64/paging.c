#include <resea/types.h>
#include <kernel/memory.h>
#include "paging.h"
#include "asm.h"


static paddr_t lookup_page_entry(struct arch_vmspace *vms, uintptr_t v, bool allocate,
                                 int attrs, uint64_t **table, int *index) {
    int idx;
    uint64_t *t;

    t = (uint64_t *) from_paddr((paddr_t) vms->pml4_addr);

    for (int i=4; i > 1; i--) {
        idx = (v >> (((i-1) * 9) + 12))  & 0x1ff;

        if (!t[idx]) {
            /* the PDPT, PD or PT is not allocated so allocate it */
            if (allocate) {
                paddr_t paddr;
                paddr = alloc_pages(PAGE_SIZE * 1, KMALLOC_NORMAL);
                t[idx] = paddr | attrs | PAGE_PRESENT;
            } else {
                BUG("the page does not exist");
                *table = NULL;
                return 0;
            }
        }

        /* go into the next level paging table */
        t = (uint64_t *) from_paddr((uint64_t) t[idx] & 0x7ffffffffffff000);
    }

    /* t is now a pointer to the PT */
    idx = (v >> 12) & 0x1ff; // idx in PT

    if (table)
        *table = t;
    if (index)
        *index = idx;

    return (t[idx] & ~(0xfff));
}


void arch_create_vmspace(struct arch_vmspace *vms) {
}


void arch_remove_vmspace(struct arch_vmspace *vms) {
}


void arch_switch_vmspace(struct arch_vmspace *vms) {
    asm_set_cr3(vms->pml4_addr);
}


void arch_link_page(struct arch_vmspace *vms, uintptr_t v, paddr_t p, size_t n,
                    int attrs) {
    int idx;
    uint64_t *table;

    /* pages for the kernel are already mapped */
    if (v > KERNEL_BASE_ADDR)
        return;

    lookup_page_entry(vms, v, true, attrs, &table, &idx);

    while(n > 0 && idx < PAGE_ENTRY_NUM) {
        table[idx] = p | attrs;
        asm_invlpg(v);
        n--;
        idx++;
        p += PAGE_SIZE;
    }

    /* pages which belongs to the different PT */
    if (n > 0)
        arch_link_page(vms, v, p, n, attrs);
}


void x64_init_paging(void) {
    STATIC_ASSERT(PAGE_SIZE >= sizeof(uint64_t) * PAGE_ENTRY_NUM,
        "Page size must be >= the size of PML4/PDPT/PD.");

    /* Construct kernel space mappings. */
    uint64_t flags = PAGE_PRESENT | PAGE_WRITABLE;
    paddr_t pml4_addr = alloc_pages(PAGE_SIZE, KMALLOC_NORMAL);
    uint64_t *pml4 = from_paddr(pml4_addr);

    paddr_t pdpt_addr = alloc_pages(PAGE_SIZE, KMALLOC_NORMAL);
    uint64_t *pdpt = from_paddr(pdpt_addr);

    for (uint64_t i = 0; i < PAGE_ENTRY_NUM; i++) {
        paddr_t pd_addr = alloc_pages(PAGE_SIZE, KMALLOC_NORMAL);
        uint64_t *pd = from_paddr(pd_addr);

        for (uint64_t j = 0; j < PAGE_ENTRY_NUM; j++) {
            paddr_t paddr = i * (1024 * 1024 * 1024) + j * (2 * 1024 * 1024);
            paddr_t vaddr = KERNEL_BASE_ADDR + paddr;
            pd[PD_INDEX(vaddr)] = paddr | flags | PAGE_ALT_SIZE;
        }

        paddr_t paddr = i * (1024 * 1024 * 1024);
        paddr_t vaddr = KERNEL_BASE_ADDR + paddr;
        pdpt[PDPT_INDEX(vaddr)] = pd_addr | flags;
    }

    pml4[PML4_INDEX(KERNEL_BASE_ADDR)] = pdpt_addr |flags;

    // Reload the created page table.
    asm_set_cr3(pml4_addr);
}
