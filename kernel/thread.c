#include <list.h>
#include "cpu.h"
#include "memory.h"
#include "thread.h"
#include "process.h"

static tid_t last_tid = 0;
static struct runqueue *runqueue;

tid_t allocate_tid(void) {
    // FIXME
    return last_tid++;
}


struct thread *thread_create(struct process *process, uintptr_t start, uintptr_t arg) {
    struct thread *thread = kmalloc(sizeof(*thread), KMALLOC_NORMAL);
    struct runqueue *rq = kmalloc(sizeof(*runqueue), KMALLOC_NORMAL);

    uintptr_t stack_size = 0x1000;
    uintptr_t stack = kmalloc(stack_size, KMALLOC_NORMAL);

    thread->tid = allocate_tid();
    thread->flags = THREAD_BLOCKED;
    arch_init_arch(&thread->arch, start, stack, stack_size);
    thread_list_append(&process->threads, thread);

    rq->thread = thread;
    runqueue_list_append(&runqueue, rq);
    return thread;
}


void thread_destroy(struct thread *thread) {
}


void thread_switch(void) {
    if (!runqueue) {
        PANIC("No threads.");
    }

    for (struct runqueue *rq = runqueue; rq != NULL; rq = rq->next) {
        int state = thread_get_state(rq->thread);
        if (state == THREAD_RUNNABLE || rq->thread != CPUVAR->current_thread) {
            if (CPUVAR->current_thread) {
                arch_switch(&CPUVAR->current_thread->arch, &rq->thread->arch);
            } else {
                CPUVAR->current_thread = rq->thread;
                arch_first_switch(&rq->thread->arch);
            }
        }
    }
}


void thread_init(void) {

    runqueue_list_init(&runqueue);
    CPUVAR->current_thread = NULL;
}
