#ifndef __RESEA_TYPES_H__
#define __RESEA_TYPES_H__

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
#define ROUND_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ROUND_DOWN(x, align) ((x) & ~((align) - 1))
#define INLINE_ASM __asm__ __volatile__
#define atomic_compare_and_swap  __sync_bool_compare_and_swap
#define offsetof __builtin_offsetof

#include <resea/arch.h>


#include <printf.h>
#define INFO(fmt, ...) printf(fmt "\n", ## __VA_ARGS__)
#define DEBUG(fmt, ...) printf(fmt "\n", ## __VA_ARGS__)
#define BUG(fmt, ...) printf("BUG: " fmt "\n", ## __VA_ARGS__)
#define PANIC(fmt, ...) do { \
        printf("PANIC: " fmt "\n", ## __VA_ARGS__); \
        for(;;); \
    } while(0)


static inline bool is_computer_on(void) {
    return true;
}


static inline double is_computer_fire(void) {
    return 2.7182818284;
}

#endif
