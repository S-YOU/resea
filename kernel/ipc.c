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

    if (cid == 0 || cid > channels_max) {
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


channel_t channel_connect(struct channel *server, struct process *client) {
    struct channel *server_side = channel_create(server->process);
    struct channel *client_side = channel_create(client);
    link_channels(server_side, client_side);
    transfer_to(server_side, server);
    return client_side->cid;
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
            DEBUG("OPENING %d %p <<<<<<", payload, ch);
            if (!ch) {
                DEBUG("copy_payload: invalid ch %d", payload);
                return 0; // TODO: return error
            }

            struct channel *new_ch = channel_create(dst);
            if (!new_ch) {
                DEBUG("copy_payload: failed to create channel");
                return 0;
            }
            DEBUG("CREATED %p <<<<<<", ch);
            if (ch->linked_to) {
                DEBUG("copy_payload: link %d %d", ch->linked_to->cid, new_ch->cid);
                new_ch->linked_to = ch->linked_to;
                ch->linked_to->linked_to = new_ch;
                close_channel(ch);
            } else {
                DEBUG("copy_payload: link %d %d", ch->cid, new_ch->cid);
                new_ch->linked_to = ch;
                ch->linked_to = new_ch;
            }

            DEBUG("RETURN %p <<<<<<", new_ch->cid);
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
    payload_t *rs
) {
    header_t error = sys_send(ch, type, a0, a1, a2, a3);
    if (error != ERROR_NONE) {
        return error;
    }

    return sys_recv(ch, rs);
}


header_t sys_replyrecv(
    channel_t client,
    header_t type,
    payload_t r0,
    payload_t r1,
    payload_t r2,
    payload_t r3,
    payload_t *rs
) {
    header_t error = sys_send(client, type, r0, r1, r2, r3);
    if (error != ERROR_NONE) {
        return error;
    }

    struct channel *server = get_channel_by_id(client)->transfer_to;
    if (!server) {
        return ERROR_CH_NOT_TRANSFERED;
    }

    return sys_recv(server->cid, rs);
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
        DEBUG("sys_send: @%d no such channel", ch);
        return ERROR_INVALID_CH;
    }

    struct channel *linked_to = src->linked_to;
    if (!linked_to) {
        DEBUG("sys_send: @%d not linked", ch);
        return ERROR_CH_NOT_LINKED;
    }

    struct thread *current_thread = CPUVAR->current_thread;
    struct channel *dst = (linked_to->transfer_to) ? linked_to->transfer_to : linked_to;

    DEBUG("sys_send: @%d.%d -> @%d.%d (type=%d.%d)",
        src->process->pid, src->cid, dst->process->pid, dst->cid,
        MSG_SERVICE_ID(type), MSG_ID(type));

    DEBUG("$$$$$$$$$$ sender locking %p %d.%d by #%d.%d **********", &dst->sender, dst->process->pid, dst->cid, CPUVAR->current_process->pid, current_thread->tid);

    while (true) {
        // Try to get the sender right.
        if (atomic_compare_and_swap(&dst->sender, NULL, current_thread)) {
            // Now we have a sender right (dst->sender == current_thread).
            break;
        }

        // XXX: lock the wait queue or current thread will never wake up

        // Another thread is sending to the destination. Add current thread
        // to wait queue. A receiver thread will resume it.
        struct waitqueue *wq = kmalloc(sizeof(*wq), KMALLOC_NORMAL);
        wq->thread = current_thread;

INFO("switch???");
        waitqueue_list_append(&dst->wq, wq);
        thread_set_state(current_thread, THREAD_BLOCKED);
        thread_switch();
    }
INFO("DO SEND");
    // Copy payloads.
    struct process *src_process = CPUVAR->current_process;
    struct process *dst_process = dst->process;
INFO("DO SEND");
    dst->sent_from = linked_to->cid;
    dst->type = type;
    dst->buffer[0] = copy_payload(PAYLOAD_TYPE(type, 0), src_process, dst_process, a0);
    dst->buffer[1] = copy_payload(PAYLOAD_TYPE(type, 1), src_process, dst_process, a1);
    dst->buffer[2] = copy_payload(PAYLOAD_TYPE(type, 2), src_process, dst_process, a2);
    dst->buffer[3] = copy_payload(PAYLOAD_TYPE(type, 3), src_process, dst_process, a3);
INFO("DO SEND");
    thread_set_state(dst->receiver, THREAD_RUNNABLE);
INFO("DO SEND");

    return ERROR_NONE;
}


header_t sys_recv(
    channel_t ch,
    payload_t *rs
) {
    struct channel *src = get_channel_by_id(ch);
    if (!src) {
        DEBUG("sys_recv: @%d no such channel", ch);
        return ERROR_INVALID_CH;
    }

    // Try to get the receiver right.
    struct thread *current_thread = CPUVAR->current_thread;
    DEBUG("$$$$$$$$$$ receiver locking %p %d.%d", &src->receiver, src->process->pid, src->cid);
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
    rs[0] = src->sent_from;
    rs[1] = src->buffer[0];
    rs[2] = src->buffer[1];
    rs[3] = src->buffer[2];
    rs[4] = src->buffer[3];
    src->receiver = NULL;
    DEBUG("$$$$$$$$$$ release %p %d.%d", &src->receiver, src->process->pid, src->cid);
    src->sender = NULL;

    return reply_type;
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
