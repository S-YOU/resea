#ifndef __IPC_H__
#define __IPC_H__

#include <kernel/types.h>
#include "thread.h"

typedef uintmax_t channel_t;
typedef uintmax_t header_t;
typedef uintmax_t payload_t;
typedef uintmax_t header_t;
typedef header_t error_t;
typedef uintmax_t usize_t;
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;
typedef int8_t i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;
typedef char * string_t;
typedef u8_t * buffer_t;

#define PAYLOAD_TYPE(type, n) (((type) >> (3 * (n))) & 0x3)
#define PAYLOAD_INLINE  0
#define PAYLOAD_CHANNEL 1

#define TYPES_OFFSET   0ULL
#define ERROR_OFFSET    24ULL
#define MINOR_ID_OFFSET 32ULL
#define MAJOR_ID_OFFSET 40ULL
#define MSGTYPE(header) ((header) >> MINOR_ID_OFFSET)
#define ERRTYPE(header) (((header) >> MAJOR_ID_OFFSET) & 0xff)

enum {
    ERROR_NONE = 0,
    ERROR_NO_MEMORY = 1,
    ERROR_INVALID_CH = 2,
    ERROR_CH_NOT_LINKED = 3,
    ERROR_CH_NOT_TRANSFERED = 4,
    ERROR_CH_IN_USE = 5,
};

struct waitqueue {
    struct waitqueue *next;
    struct thread *thread;
};

DEFINE_LIST(waitqueue, struct waitqueue)

struct channel {
    int flags;
    channel_t cid;
    struct process *process;
    struct channel *linked_to;
    struct channel *transfer_to;
    struct thread *receiver;
    struct thread *sender;
    struct waitqueue *wq;
    payload_t sent_from, type, buffer[5];
};

struct channel *channel_create(struct process *process);
error_t channel_connect(struct channel *server, struct process *client);


header_t sys_recv(
    channel_t ch,
    channel_t *from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
);

header_t sys_call(
    channel_t ch,
    header_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3,
    payload_t *r0,
    payload_t *r1,
    payload_t *r2,
    payload_t *r3
);

header_t sys_replyrecv(
    channel_t client,
    header_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    channel_t *sent_from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
);

static inline header_t ipc_recv(
    channel_t ch,
    channel_t *from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
) {
    return sys_recv(ch, from, a0, a1, a2, a3);
}

static inline header_t ipc_call(
    channel_t ch,
    header_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3,
    payload_t *r0,
    payload_t *r1,
    payload_t *r2,
    payload_t *r3
) {
    return sys_call(ch, type, a0, a1, a2, a3, r0, r1, r2, r3);
}

static inline header_t ipc_replyrecv(
    channel_t server,
    header_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    channel_t *client,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
) {
    return sys_replyrecv(server, type, r0, r1, r2, r3, client, a0, a1, a2, a3);
}

#endif
