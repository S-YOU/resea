#ifndef __THREAD_H__
#define __THREAD_H__

#include <kernel/types.h>
#include "cpu.h"
#include "list.h"

typedef u32_t tid_t;

#define THREAD_RUNNABLE 1
#define THREAD_BLOCKED 2
#define KERNEL_STACK_SIZE 8192

struct process;
struct thread {
    struct thread *next;
    struct process *process;
    u32_t flags;
    tid_t tid;
    struct arch_thread arch;
    int resumed_count; /* runnable if resumed_count > 0 */
};

struct runqueue {
    struct runqueue *next;
    struct thread *thread;
};

DEFINE_LIST(thread, struct thread)
DEFINE_LIST(runqueue, struct runqueue)

struct process;

tid_t allocate_tid(void);
struct thread *thread_create(struct process *process, uptr_t start, uptr_t arg);
void thread_destroy(struct thread *thread);
NORETURN void thread_destroy_current(void);
void thread_switch(void);
void thread_switch_to(struct thread *thread);
void thread_init(void);
void thread_resume(struct thread *thread);
void thread_block(struct thread *thread);
void thread_block_current(void);

static inline int thread_get_state(struct thread *thread) {
    return thread->flags & 3;
}

#endif
