#ifndef __RESEA_H__
#define __RESEA_H__

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
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


static inline double is_computer_on_fire(void) {
    return 2.7182818284;
}

#define MAJOR_ID_OFFSET 40ULL
#define MINOR_ID_OFFSET 32ULL
#define TYPES_OFFSET    8ULL
#define ERRROR_OFFSET   0ULL

// TODO: interface generator
#define SERVICE_ID(major, minor) (((major) << MAJOR_ID_OFFSET) | ((minor) << MINOR_ID_OFFSET))
#define DISCOVERY_SERVICE  SERVICE_ID(1ULL, 0ULL)
#define BENCHMARK_SERVICE  SERVICE_ID(2ULL, 0ULL)
#define DISCOVERY_REGISTER SERVICE_ID(1ULL, 1ULL)
#define BENCHMARK_REQUEST  SERVICE_ID(2ULL, 1ULL)
#define BENCHMARK_REPLY    SERVICE_ID(2ULL, 2ULL)

channel_t ipc_open(void);
type_t ipc_send(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3
);

type_t ipc_recv(
    channel_t ch,
    channel_t *from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
);

type_t ipc_call(
    channel_t ch,
    type_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3,
    payload_t *r0,
    payload_t *r1,
    payload_t *r2,
    payload_t *r3
);

type_t ipc_replyrecv(
    channel_t server,
    channel_t *client,
    type_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
);

type_t ipc_discard(
    payload_t ool0,
    payload_t ool1,
    payload_t ool2,
    payload_t ool3
);

type_t ipc_link(channel_t ch0, channel_t ch1);
type_t ipc_transfer(channel_t ch, channel_t dest);
channel_t ipc_connect(channel_t server);

#endif
