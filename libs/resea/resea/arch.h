#ifndef __RESEA_ARCH_H__
#define __RESEA_ARCH_H__

#include <arch_types.h>
typedef uintmax_t size_t;

void arch_init(void);
void arch_early_init(void);
void arch_putchar(char ch);

void arch_init_arch(struct arch_thread *arch, uintptr_t start, uintmax_t arg,
                    uintptr_t stack, size_t stack_size);
void arch_switch(struct arch_thread *prev, struct arch_thread *next);
NORETURN void arch_first_switch(struct arch_thread *next);

#endif
