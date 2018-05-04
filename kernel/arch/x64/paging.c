#include <kernel/types.h>
#include <kernel/memory.h>
#include <string.h>
#include "paging.h"
#include "asm.h"


uint64_t *kernel_pml4 = NULL;

static void lookup_page_entry(struct arch_vmspace *vms, uintptr_t v, bool allocate,
                              int attrs, uint64_t **table, int *index) {
    int idx;
    uint64_t *entries = (uint64_t *) from_paddr((paddr_t) vms->pml4_addr);

    for (int i = 4; i > 1; i--) {
        idx = (v >> (((i - 1) * 9) + 12)) & 0x1ff;
        if (!entries[idx]) {
            /* the PDPT, PD or PT is not allocated so allocate it */
            if (allocate) {
                paddr_t paddr;
                paddr = alloc_pages(PAGE_SIZE, KMALLOC_NORMAL);
                INFO("link: %p   ", paddr | attrs);
                entries[idx] = paddr;
            } else {
                *table = NULL;
                BUG("the page does not exist");
                return;
            }
        }

        entries[idx] = (entries[idx] & ~PAGE_ATTRS) | attrs | PAGE_PRESENT;

        /* go into the next level paging table */
        entries = (uint64_t *) from_paddr((uint64_t) entries[idx] & 0x7ffffffffffff000);
    }

    /* table is now a pointer to the PT */
    idx = (v >> 12) & 0x1ff; // idx in PT

    *table = entries;
    *index = idx;
}


void arch_create_vmspace(struct arch_vmspace *vms) {
    size_t pml4_size = PAGE_SIZE;
    paddr_t pml4_addr = alloc_pages(pml4_size, KMALLOC_NORMAL);
    uint64_t *pml4 = from_paddr(pml4_addr);

    // Copy kernel space page entries.
    memcpy(pml4, kernel_pml4, pml4_size);

    vms->pml4_addr = pml4_addr;
}


void arch_destroy_vmspace(UNUSED struct arch_vmspace *vms) {
}


void arch_switch_vmspace(struct arch_vmspace *vms) {

    asm_set_cr3(vms->pml4_addr);
}


void arch_link_page(struct arch_vmspace *vms, uintptr_t vaddr, paddr_t paddr, size_t num,
                    int attrs) {
    int idx;
    uint64_t *table;

link_to_next_pt:

    /* pages for the kernel are already mapped */
    if (vaddr > KERNEL_BASE_ADDR)
        return;

    lookup_page_entry(vms, vaddr, true, attrs, &table, &idx);

    while(num > 0 && idx < PAGE_ENTRY_NUM) {
        table[idx] = paddr | attrs | PAGE_PRESENT;
        INFO("tabl: %p   ", paddr | attrs);
        asm_invlpg(vaddr);
        num--;
        idx++;
        paddr += PAGE_SIZE;
    }

    // Pages which belongs to the next PT,
    if (num > 0) {
        goto link_to_next_pt;
    }
}


void x64_init_paging(void) {
    STATIC_ASSERT(PAGE_SIZE >= sizeof(uint64_t) * PAGE_ENTRY_NUM,
        "Page size must be >= the size of PML4/PDPT/PD.");

    /* Construct kernel space mappings. */
    uint64_t flags = PAGE_PRESENT | PAGE_WRITABLE;
    paddr_t pml4_addr = alloc_pages(PAGE_SIZE, KMALLOC_NORMAL);
    kernel_pml4 = from_paddr(pml4_addr);

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

    kernel_pml4[PML4_INDEX(KERNEL_BASE_ADDR)] = pdpt_addr |flags;

    // Reload the created page table.
    asm_set_cr3(pml4_addr);
}
