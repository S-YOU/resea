#ifndef __IPC_H__
#define __IPC_H__

#include <kernel/types.h>
#include "thread.h"

typedef uintmax_t channel_t;
typedef uintmax_t type_t;
typedef uintmax_t payload_t;

struct waitqueue {
    struct waitqueue *next;
    struct thread *thread;
};

struct channel {
    int flags;
    struct channel *linked_to;
    struct channel *transfer_to;
    struct thread *receiver;
    struct thread *sender;
    struct waitqueue *wq;
};

#endif
