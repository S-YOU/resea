#include <string.h>
#include "process.h"
#include "thread.h"
#include "memory.h"
#include "elf.h"


void elf_create_process(const void *image, UNUSED size_t length, pager_t *pager, void *pager_arg) {
    struct elf64_ehdr *ehdr = (struct elf64_ehdr *) image;

    /* check out the magic number */
    if (strncmp((const char *) &ehdr->e_ident, ELF_MAGIC, 4) != 0) {
        INFO("%s: invalid elf magic, skipping...", image);
        return;
    }

    if (ehdr->e_ident[4] != ELFCLASS64 || ehdr->e_machine != EM_X86_64) {
        INFO("%s: unsupported type, skipping...", image);
        return;
    }

    struct process *process = process_create();
    INFO("elf: created process #%d", process->pid);

    /* Load program headers. */
    for (int i = 0; i < ehdr->e_phnum; i++) {
        struct elf64_phdr * phdr = (struct elf64_phdr *) ((uintptr_t) image + ehdr->e_phoff + (ehdr->e_phentsize * i));
        if (phdr->p_type == PT_LOAD) {
            INFO("#%d: loading a segment addr=%p, file_offset=%p, size=%dKB",
                process->pid, phdr->p_vaddr, phdr->p_offset, phdr->p_filesz);

            int flags = PAGE_USER;
            flags |= (phdr->p_flags & PF_W)? PAGE_WRITABLE : 0;

            add_vmarea(&process->vms, phdr->p_vaddr, phdr->p_offset,
                phdr->p_filesz, flags, pager, pager_arg);
        }
    }

    struct thread *thread = thread_create(process, ehdr->e_entry, 0);
    thread_set_state(thread, THREAD_RUNNABLE);
}
