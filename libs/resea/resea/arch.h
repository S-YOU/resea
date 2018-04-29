#ifndef __RESEA_ARCH_H__
#define __RESEA_ARCH_H__

#include <arch_types.h>
typedef uintmax_t size_t;

void arch_init(void);
void arch_early_init(void);
void arch_putchar(char ch);
void arch_idle(void);
void arch_create_thread(struct arch_thread *arch, bool is_kernel_thread,
                    uintptr_t start, uintmax_t arg,
                    uintptr_t stack, size_t stack_size);
void arch_destroy_thread(struct arch_thread *arch);
void arch_switch(struct arch_thread *prev, struct arch_thread *next);

#endif
