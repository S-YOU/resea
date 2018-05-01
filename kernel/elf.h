#ifndef __ELF_H__
#define __ELF_H__

#include "types.h"
#include "memory.h"

enum elf_archtype{
  ELF_ARCH_X86_64
};

#define EI_NIDENT   16
#define EI_MAG0     0x7f
#define EI_MAG1     'E'
#define EI_MAG2     'L'
#define EI_MAG3     'F'
#define ELFCLASS64  2
#define EM_X86_64   62
#define PT_LOAD     1
#define PF_R        4
#define PF_W        2
#define PF_X        1

struct elf64_ehdr{
  uint8_t  e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct elf64_phdr {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

struct elf64_shdr {
   uint32_t sh_name;
   uint32_t sh_type;
   uint64_t sh_flags;
   uint64_t sh_addr;
   uint64_t sh_offset;
   uint64_t sh_size;
   uint32_t sh_link;
   uint32_t sh_info;
   uint64_t sh_addralign;
   uint64_t sh_entsize;
};

void elf_create_process(const void *image, size_t length, pager_t *pager, void *pager_arg);

#endif
