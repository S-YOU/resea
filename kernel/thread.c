#include <list.h>
#include "cpu.h" // FIXME
#include "memory.h"
#include "thread.h"
#include "process.h"

static tid_t last_tid = 0;
static struct runqueue *runqueue;
struct thread *idle_thread = NULL;


tid_t allocate_tid(void) {
    // FIXME
    return last_tid++;
}


struct thread *thread_create(struct process *process, uintptr_t start, uintptr_t arg) {
    bool is_kernel_thread = process == kernel_process;
    struct thread *thread = kmalloc(sizeof(*thread), KMALLOC_NORMAL);
    struct runqueue *rq = kmalloc(sizeof(*runqueue), KMALLOC_NORMAL);

    uintptr_t stack;
    size_t stack_size;
    if (is_kernel_thread) {
        stack_size = 0x1000;
        stack = (uintptr_t) kmalloc(stack_size, KMALLOC_NORMAL);
    } else {
        stack_size = 64 * 1024;
        stack = 0xc0000000;
    }

    thread->process = process;
    thread->tid = allocate_tid();
    thread->flags = THREAD_BLOCKED;
    arch_create_thread(&thread->arch, is_kernel_thread,
        start, arg, stack, stack_size);
    thread_list_append(&process->threads, thread);

    rq->thread = thread;
    runqueue_list_append(&runqueue, rq);
    return thread;
}


void thread_destroy(struct thread *thread) {
    if (thread == idle_thread) {
        PANIC("idle thread can't be destroyied");
    }

    // XXX: lock
    for (struct runqueue *rq = runqueue; rq != NULL; rq = rq->next) {
        if (rq->thread == thread) {
            runqueue_list_remove(&runqueue, rq);
            kfree(rq);
            break;
        }
    }

    thread_list_remove(&thread->process->threads, thread);
    arch_destroy_thread(&thread->arch);
    kfree(thread);
}


NORETURN void thread_destroy_current(void) {
    struct thread *thread = CPUVAR->current_thread;
    if (thread == idle_thread) {
        PANIC("idle thread can't be destroyied");
    }

    PANIC("%s: not yet implemented", __func__);
}


void thread_switch(void) {
    // Runqueue will never be empty since idle runs forever until
    // is_computer_on() returns true.
    //
    // Note that we can't switch into the idle thread on the first call.
    // call because the current context will become its context.
    //
    struct runqueue *rq = CPUVAR->current_runqueue->next;

    // XXX: lock
    while (rq) {
        int state = thread_get_state(rq->thread);
        if (state == THREAD_RUNNABLE && rq->thread != CPUVAR->current_thread) {
            struct thread *current_thread = CPUVAR->current_thread;
            CPUVAR->current_thread = rq->thread;
            CPUVAR->current_runqueue = rq;
            INFO("%s: %d RIP=%p RSP=%p", __func__, rq->thread->tid, rq->thread->arch.rip, rq->thread->arch.rsp);
            arch_switch_vmspace(&current_thread->process->vms.arch);
            arch_switch(&current_thread->arch, &rq->thread->arch);
            return;
        }

        rq = rq->next;
    }

    rq = runqueue->next; // Skip idle thread.
    while (rq) {
        int state = thread_get_state(rq->thread);
        if (state == THREAD_RUNNABLE && rq->thread != CPUVAR->current_thread) {
            struct thread *current_thread = CPUVAR->current_thread;
            CPUVAR->current_thread = rq->thread;
            CPUVAR->current_runqueue = rq;
            INFO("%s: %d RIP=%p RSP=%p", __func__, rq->thread->tid, rq->thread->arch.rip, rq->thread->arch.rsp);
            arch_switch_vmspace(&current_thread->process->vms.arch);
            arch_switch(&current_thread->arch, &rq->thread->arch);
            return;
        }

        rq = rq->next;
    }

    if (thread_get_state(CPUVAR->current_thread) == THREAD_RUNNABLE) {
        // No another thread to run. Resume the current thread.
        return;
    }

    // No threads are runnable. Resume the idle thread. Assuming `runqueue` points
    // to the idle thread.
    struct thread *current_thread = CPUVAR->current_thread;
    CPUVAR->current_thread = runqueue->thread;
    CPUVAR->current_runqueue = runqueue;
    arch_switch(&current_thread->arch, &runqueue->thread->arch);

}


void thread_init(void) {

    runqueue_list_init(&runqueue);
    CPUVAR->current_runqueue = NULL;

    // Create an idle thread. We specify NULL as handler because it won't
    // be used.
    idle_thread = thread_create(kernel_process, (uintptr_t) NULL, 0);
    CPUVAR->current_thread = idle_thread;
    CPUVAR->current_runqueue = runqueue;
}
