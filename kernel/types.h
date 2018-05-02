#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool;

#define NULL ((void *) 0)
#define false 0
#define true  1
#define STATIC_ASSERT _Static_assert
#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define DEPRECATED __attribute__((deprecated))
#define UNREACHABLE __builtin_unreachable()
#define ROUND_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ROUND_DOWN(x, align) ((x) & ~((align) - 1))
#define INLINE_ASM __asm__ __volatile__
#define atomic_compare_and_swap  __sync_bool_compare_and_swap
#define offsetof __builtin_offsetof
#define va_list __builtin_va_list
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

#include <arch_types.h>
typedef uintmax_t size_t;
typedef uintmax_t off_t;

void arch_init(void);
void arch_early_init(void);
void arch_putchar(char ch);
void arch_idle(void);
void arch_create_thread(struct arch_thread *arch, bool is_kernel_thread,
                    uintptr_t start, uintmax_t arg,
                    uintptr_t stack, size_t stack_size);
void arch_destroy_thread(struct arch_thread *arch);
void arch_switch(struct arch_thread *prev, struct arch_thread *next);

void arch_create_vmspace(struct arch_vmspace *vms);
void arch_destroy_vmspace(struct arch_vmspace *vms);
void arch_switch_vmspace(struct arch_vmspace *vms);
void arch_link_page(struct arch_vmspace *vms, uintptr_t vaddr, paddr_t paddr, size_t num,
                    int attrs);

#include <printf.h>
#define INFO(fmt, ...) printf(fmt "\n", ## __VA_ARGS__)
#define DEBUG(fmt, ...) printf(fmt "\n", ## __VA_ARGS__)
#define BUG(fmt, ...) printf("BUG: " fmt "\n", ## __VA_ARGS__)
#define PANIC(fmt, ...) do { \
        printf("PANIC: " fmt "\n", ## __VA_ARGS__); \
        for(;;); \
    } while(0)

#endif
