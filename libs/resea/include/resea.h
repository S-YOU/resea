#ifndef __RESEA_H__
#define __RESEA_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool;

#define NULL ((void *) 0)
#define false 0
#define true  1
#define offsetof __builtin_offsetof
#define STATIC_ASSERT _Static_assert
#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define DEPRECATED __attribute__((deprecated))
#define UNREACHABLE __builtin_unreachable()
#define va_list __builtin_va_list
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

#include <arch.h>

static inline bool is_computer_on(void) {
    return true;
}


static inline double is_computer_fire(void) {
    return 2.7182818284;
}

#endif
