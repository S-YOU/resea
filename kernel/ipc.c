#include "cpu.h"
#include "thread.h"
#include "process.h"
#include "ipc.h"


static inline void link_channels(struct channel *ch1, struct channel *ch2) {
    ch1->linked_to = ch2;
    ch2->linked_to = ch1;
}


static inline void transfer_to(struct channel *from, struct channel *to) {
    from->transfer_to = to;
}


static inline struct channel *get_channel_by_id(channel_t cid) {
    size_t channels_max = CPUVAR->current_process->channels_max;
    struct channel *channels = (struct channel *) &CPUVAR->current_process->channels;

    if (cid > channels_max) {
        return NULL;
    }

    struct channel *ch = &channels[cid - 1];
    return (ch->flags == 0) ? NULL : ch;
}

struct channel *channel_create(struct process *process) {
    kmutex_state_t state = kmutex_lock_irq_disabled(&process->lock);
    size_t channels_max = process->channels_max;
    for (size_t i = 0; i < channels_max; i++) {
        struct channel *ch = &process->channels[i];
        if (ch->flags == 0) {
            ch->flags = 1;
            ch->cid = i + 1;
            ch->process = process;
            ch->linked_to = NULL;
            ch->transfer_to = NULL;
            ch->receiver = NULL;
            ch->sender = NULL;
            waitqueue_list_init(&ch->wq);
            kmutex_unlock_restore_irq(&process->lock, state);
            return ch;
        }
    }

    kmutex_unlock_restore_irq(&process->lock, state);
    return NULL;
}


error_t channel_connect(struct channel *server, struct process *client) {
    struct channel *server_side = channel_create(server->process);
    struct channel *client_side = channel_create(client);
    link_channels(server_side, client_side);
    transfer_to(server_side, server);
    return ERROR_NONE;
}


static inline void close_channel(struct channel *ch) {
    ch->flags = 0;
}


static payload_t copy_payload(
    int type,
    struct process *src,
    struct process *dst,
    payload_t payload
) {

    switch (type) {
        case PAYLOAD_INLINE:
            return payload;
        case PAYLOAD_CHANNEL: {
            struct channel *ch = get_channel_by_id(payload);
            struct channel *new_ch = channel_create(dst);

            if (ch->linked_to) {
                ch->linked_to->linked_to = new_ch;
                close_channel(ch);
            } else {
                ch->linked_to = new_ch;
            }

            return new_ch->cid;
        }
    }

    // Invalid payload type.
    return 0;
}

channel_t sys_open(void) {
    struct channel *ch = channel_create(CPUVAR->current_process);
    if(!ch) {
        DEBUG("sys_open: failed to allocate #%d", CPUVAR->current_process->pid);
        return ERROR_NO_MEMORY;
    }

    DEBUG("sys_open: #%d allocate @%d", CPUVAR->current_process->pid, ch->cid);
    return ch->cid;
}


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
) {
    struct channel *src = get_channel_by_id(ch);
    if (!src) {
        DEBUG("sys_call: @%d no such channel", ch);
        return ERROR_INVALID_CH;
    }

    struct channel *dst = src->linked_to;
    if (!dst) {
        DEBUG("sys_call: @%d not linked", ch);
        return ERROR_CH_NOT_LINKED;
    }

    // Try to get the receiver right.
    struct thread *current_thread = CPUVAR->current_thread;
    if (!atomic_compare_and_swap(&src->receiver, NULL, current_thread)) {
        return ERROR_CH_IN_USE;
    }

    struct channel *real_dst = (dst->transfer_to) ? dst->transfer_to : dst;
    DEBUG("sys_call: @%d.%d -> @%d.%d (type=%d.%d)",
        src->process->pid, src->cid, real_dst->process->pid, real_dst->cid,
        MSG_SERVICE_ID(type), MSG_ID(type));

    while (true) {
        // Try to get the sender right.
        if (atomic_compare_and_swap(&real_dst->sender, NULL, current_thread)) {
            // Now we have a sender right (real_dst->sender == current_thread).
            break;
        }

        // XXX: lock the wait queue or current thread will never wake up

        // Another thread is sending to the destination. Add current thread
        // to wait queue. A receiver thread will resume it.
        struct waitqueue *wq = kmalloc(sizeof(*wq), KMALLOC_NORMAL);
        wq->thread = current_thread;

        waitqueue_list_append(&real_dst->wq, wq);
        thread_set_state(current_thread, THREAD_BLOCKED);
        thread_set_state(real_dst->receiver, THREAD_RUNNABLE);
        thread_switch();
    }

    // Copy payloads.
    struct process *src_process = CPUVAR->current_process;
    struct process *dst_process = dst->process;
    real_dst->sent_from = dst->cid;
    real_dst->type = type;
    real_dst->buffer[0] = copy_payload(PAYLOAD_TYPE(type, 0), src_process, dst_process, a0);
    real_dst->buffer[1] = copy_payload(PAYLOAD_TYPE(type, 1), src_process, dst_process, a1);
    real_dst->buffer[2] = copy_payload(PAYLOAD_TYPE(type, 2), src_process, dst_process, a2);
    real_dst->buffer[3] = copy_payload(PAYLOAD_TYPE(type, 3), src_process, dst_process, a3);

    // Perform a context switch to the destination.
    thread_set_state(current_thread, THREAD_BLOCKED);
    thread_set_state(real_dst->receiver, THREAD_RUNNABLE);
    thread_switch_to(real_dst->receiver);

    // Receiver sent a reply message and resumed the sender thread. Do recv
    // work.
    header_t reply_type = src->type;
    *r0 = src->buffer[0];
    *r1 = src->buffer[1];
    *r2 = src->buffer[2];
    *r3 = src->buffer[3];
    src->receiver = NULL;
    real_dst->sender = NULL;
    thread_set_state(real_dst->sender, THREAD_RUNNABLE);

    return reply_type;
}


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
) {
    struct channel *src = get_channel_by_id(client);
    if (!src) {
        DEBUG("sys_replyrecv: @%d no such channel", client);
        return ERROR_INVALID_CH;
    }

    struct channel *dst = src->linked_to;
    if (!dst) {
        DEBUG("sys_replyrecv: @%d not linked", client);
        return ERROR_CH_NOT_LINKED;
    }

    struct channel *server = src->transfer_to;
    if (!server) {
        DEBUG("sys_replyrecv: @%d is not transfered to a server", client);
        return ERROR_CH_NOT_TRANSFERED;
    }

    // Try to get the receiver right.
    struct thread *current_thread = CPUVAR->current_thread;
    if (!atomic_compare_and_swap(&server->receiver, NULL, current_thread)) {
        return ERROR_CH_IN_USE;
    }

    struct channel *real_dst = (dst->transfer_to) ? dst->transfer_to : dst;

    DEBUG("sys_replyto: @%d.%d -> @%d.%d (type=%d.%d)",
        src->process->pid, src->cid, real_dst->process->pid, real_dst->cid,
        MSG_SERVICE_ID(type), MSG_ID(type));

    while (true) {
        // Try to get the sender right.
        if (atomic_compare_and_swap(&real_dst->sender, NULL, current_thread)) {
            // Now we have a sender right (real_dst->sender == current_thread).
            break;
        }

        // XXX: lock the wait queue or current thread will never wake up

        // Another thread is sending to the destination. Add current thread
        // to wait queue. A receiver thread will resume it.
        struct waitqueue *wq = kmalloc(sizeof(*wq), KMALLOC_NORMAL);
        wq->thread = current_thread;

        waitqueue_list_append(&real_dst->wq, wq);
        thread_set_state(current_thread, THREAD_BLOCKED);
        thread_switch();
    }

    // Copy payloads.
    struct process *src_process = CPUVAR->current_process;
    struct process *dst_process = dst->process;
    real_dst->sent_from = dst->cid;
    real_dst->type = type;
    real_dst->buffer[0] = copy_payload(PAYLOAD_TYPE(type, 0), src_process, dst_process, r0);
    real_dst->buffer[1] = copy_payload(PAYLOAD_TYPE(type, 1), src_process, dst_process, r1);
    real_dst->buffer[2] = copy_payload(PAYLOAD_TYPE(type, 2), src_process, dst_process, r2);
    real_dst->buffer[3] = copy_payload(PAYLOAD_TYPE(type, 3), src_process, dst_process, r3);

    // Perform a context switch to the destination.
    thread_set_state(current_thread, THREAD_BLOCKED);
    thread_switch_to(real_dst->receiver);

    // Receiver sent a reply message and resumed the sender thread. Do recv
    // work.
    header_t reply_type = server->type;
    *sent_from = server->sent_from;
    *a0 = server->buffer[0];
    *a1 = server->buffer[1];
    *a2 = server->buffer[2];
    *a3 = server->buffer[3];
    src->receiver = NULL;
    real_dst->sender = NULL;
    thread_set_state(real_dst->sender, THREAD_RUNNABLE);

    return reply_type;
}


header_t sys_recv(
    channel_t ch,
    channel_t *from,
    payload_t *a0,
    payload_t *a1,
    payload_t *a2,
    payload_t *a3
) {
    struct channel *src = get_channel_by_id(ch);
    if (!src) {
        DEBUG("sys_recv: @%d no such channel", ch);
        return ERROR_INVALID_CH;
    }

    // Try to get the receiver right.
    struct thread *current_thread = CPUVAR->current_thread;
    if (!atomic_compare_and_swap(&src->receiver, NULL, current_thread)) {
        return ERROR_CH_IN_USE;
    }

    thread_set_state(current_thread, THREAD_BLOCKED);

    // Resume a thread in the wait queue.
    struct waitqueue *wq = waitqueue_list_pop(&src->wq);
    if (wq != NULL) {
        struct thread *sender = wq->thread;
        kfree(wq);
        thread_set_state(sender, THREAD_RUNNABLE);
        thread_switch_to(sender);
    } else {
        // No threads in the queue.
        thread_switch();
    }


    // Receiver sent a reply message and resumed the sender thread. Do recv
    // work.
    header_t reply_type = src->type;
    *from = src->sent_from;
    *a0 = src->buffer[0];
    *a1 = src->buffer[1];
    *a2 = src->buffer[2];
    *a3 = src->buffer[3];
    src->receiver = NULL;
    thread_set_state(src->sender, THREAD_RUNNABLE);

    return reply_type;
}

header_t sys_send(
    channel_t ch,
    header_t type,
    payload_t a0,
    payload_t a1,
    payload_t a2,
    payload_t a3
) {

    struct channel *src = get_channel_by_id(ch);
    if (!src) {
        DEBUG("sys_call: @%d no such channel", ch);
        return ERROR_INVALID_CH;
    }

    struct channel *dst = src->linked_to;
    if (!dst) {
        DEBUG("sys_call: @%d not linked", ch);
        return ERROR_CH_NOT_LINKED;
    }

    // Try to get the receiver right.
    struct thread *current_thread = CPUVAR->current_thread;
    if (!atomic_compare_and_swap(&src->receiver, NULL, current_thread)) {
        return ERROR_CH_IN_USE;
    }

    struct channel *real_dst = (dst->transfer_to) ? dst->transfer_to : dst;

    DEBUG("sys_send: @%d.%d -> @%d.%d (type=%d.%d)",
        src->process->pid, src->cid, real_dst->process->pid, real_dst->cid,
        MSG_SERVICE_ID(type), MSG_ID(type));

    while (true) {
        // Try to get the sender right.
        if (atomic_compare_and_swap(&real_dst->sender, NULL, current_thread)) {
            // Now we have a sender right (real_dst->sender == current_thread).
            break;
        }

        // XXX: lock the wait queue or current thread will never wake up

        // Another thread is sending to the destination. Add current thread
        // to wait queue. A receiver thread will resume it.
        struct waitqueue *wq = kmalloc(sizeof(*wq), KMALLOC_NORMAL);
        wq->thread = current_thread;

        waitqueue_list_append(&real_dst->wq, wq);
        thread_set_state(current_thread, THREAD_BLOCKED);
        thread_switch();
    }

    // Copy payloads.
    struct process *src_process = CPUVAR->current_process;
    struct process *dst_process = dst->process;
    real_dst->sent_from = dst->cid;
    real_dst->type = type | 0xaaaaaaaa;
    real_dst->buffer[0] = copy_payload(PAYLOAD_TYPE(type, 0), src_process, dst_process, a0);
    real_dst->buffer[1] = copy_payload(PAYLOAD_TYPE(type, 1), src_process, dst_process, a1);
    real_dst->buffer[2] = copy_payload(PAYLOAD_TYPE(type, 2), src_process, dst_process, a2);
    real_dst->buffer[3] = copy_payload(PAYLOAD_TYPE(type, 3), src_process, dst_process, a3);

    // Perform a context switch to the destination.
    thread_set_state(current_thread, THREAD_BLOCKED);
    thread_set_state(real_dst->receiver, THREAD_RUNNABLE);
    thread_switch_to(real_dst->receiver);
    real_dst->sender = NULL;
}

channel_t sys_connect(channel_t server) {
    struct channel *ch = get_channel_by_id(server);
    if (!ch) {
        DEBUG("sys_connect: @%d no such channel", server);
        return ERROR_INVALID_CH;
    }

    struct channel *linked_to = ch->linked_to;
    if (!linked_to) {
        DEBUG("sys_connect: @%d not linked", ch);
        return ERROR_CH_NOT_LINKED;
    }

    return channel_connect(linked_to, CPUVAR->current_process);
}


header_t invalid_syscall(void) {
    INFO("invalid syscall");
    return 0;
}
