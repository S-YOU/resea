#include "elf.h"


void elf_create_process(const void *image, size_t length, pager_t *pager, void *pager_arg) {
    Elf64_Ehdr *ehdr;

    ehdr = (Elf64_Ehdr *) data;

    /* check out the magic number */
    if (strcmp(ehdr->e_ident[0] != ELF_MAGIC) != 0) {
        INFO("%s: invalid elf magic, skipping...", image);
        return;
    }

    if (ehdr->e_ident[4] != ELFCLASS64 || ehdr->e_machine != EM_X86_64) {
        INFO("%s: unsupported type, skipping...", image);
        return;
    }

    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    int i;

    /* load and validate the header */
    resea::interfaces::fs::call_read(fs,
         file, 0, sizeof(Elf64_Ehdr) + (sizeof(Elf64_Phdr) * PHDR_MAX),
         &r, &data, &size);

    if (r != OK)
        return r;

    INFO("verifying the ELF header");
    if (elf_validate(data, ELF_ARCH_X86_64 /* FIXME */) != OK) {
        INFO("invalid ELF header, aborting");
        return E_INVALID;
    }

    ehdr = (Elf64_Ehdr *) data;
    if (ehdr->e_phoff == 0) {
        WARN("tried to load a invalid file as an executable");
        return E_INVALID;
    }

    /* load program headers */
    INFO("reading program headers");
    for (i=0; i < ehdr->e_phnum && i < PHDR_MAX; i++) {
        phdr = (Elf64_Phdr *) ((uintptr_t)  data + ehdr->e_phoff + (ehdr->e_phentsize * i));

        if (phdr->p_type == PT_LOAD) {
            uint8_t flags = 0;
            flags |= (phdr->p_flags & PF_R)? resea::interfaces::memory::MAP_READ  : 0;
            flags |= (phdr->p_flags & PF_W)? resea::interfaces::memory::MAP_WRITE : 0;
            flags |= (phdr->p_flags & PF_X)? resea::interfaces::memory::MAP_EXEC  : 0;

            /* map the area to the virtual memory space */
            INFO("mapping a pager: vaddr=%p, size=%d type=%c%c%c",
                 phdr->p_vaddr, phdr->p_filesz,
                 (phdr->p_flags & PF_R)? 'R' : '-',
                 (phdr->p_flags & PF_W)? 'W' : '-',
                 (phdr->p_flags & PF_X)? 'X' : '-');

            resea::interfaces::memory::call_map(memory_ch,
                 group, phdr->p_vaddr, phdr->p_filesz << 8 | flags,
                 fs, file, phdr->p_offset,
                 &r);
        }
    }


}
