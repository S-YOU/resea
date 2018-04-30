#ifndef __THREAD_H__
#define __THREAD_H__

#include <resea/types.h>
#include <list.h>

typedef uint32_t tid_t;

#define THREAD_RUNNABLE 1
#define THREAD_BLOCKED 2
#define KERNEL_STACK_SIZE 8192

struct process;
struct thread {
    struct thread *next;
    struct process *process;
    uint32_t flags;
    tid_t tid;
    struct arch_thread arch;
};

struct runqueue {
    struct runqueue *next;
    struct thread *thread;
};

DEFINE_LIST(thread, struct thread)
DEFINE_LIST(runqueue, struct runqueue)

static inline int thread_get_state(struct thread *thread) {
    return thread->flags & 3;
}

static inline void thread_set_state(struct thread *thread, int state) {
    thread->flags = (thread->flags & ~3) | state;
}

struct process;

tid_t allocate_tid(void);
struct thread *thread_create(struct process *process, uintptr_t start, uintptr_t arg);
void thread_destroy(struct thread *thread);
NORETURN void thread_destroy_current(void);
void thread_switch(void);
void thread_init(void);

#endif
