#ifndef __IPC_H__
#define __IPC_H__

#include <kernel/types.h>
#include "thread.h"

typedef uintmax_t channel_t;
typedef uintmax_t type_t;
typedef uintmax_t payload_t;

#define MSG_TYPE(type, n) (((type) >> (3 * n)) & 0x3)
#define PAYLOAD_INLINE  0
#define PAYLOAD_CHANNEL 1

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

enum {
    ERR_NO_MEMORY = 1,
    ERR_INVALID_CH = 2,
    ERR_CH_NOT_LINKED = 3,
    ERR_CH_NOT_TRANSFERED = 4,
    ERR_CH_IN_USE = 5,
};

struct channel *channel_create(struct process *process);

#endif
