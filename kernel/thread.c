#include "list.h"
#include "cpu.h" // FIXME
#include "memory.h"
#include "thread.h"
#include "process.h"

static tid_t last_tid = 1;
static struct runqueue *runqueue;
struct thread *idle_thread = NULL;


tid_t allocate_tid(void) {
    // FIXME
    return last_tid++;
}


struct thread *thread_create(struct process *process, uptr_t start, uptr_t arg) {
    bool is_kernel_thread = process == kernel_process;
    struct thread *thread = kmalloc(sizeof(*thread), KMALLOC_NORMAL);
    struct runqueue *rq = kmalloc(sizeof(*rq), KMALLOC_NORMAL);

    uptr_t stack;
    size_t stack_size;
    if (is_kernel_thread) {
        stack_size = 0x1000;
        stack = (uptr_t) kmalloc(stack_size, KMALLOC_NORMAL);
    } else {
        stack = process->next_stack_start;
        stack_size = 4 * PAGE_SIZE;

        process->next_stack_start = stack + stack_size;
        add_vmarea(&process->vms, stack, 0, stack_size,
            PAGE_USER | PAGE_WRITABLE, zeroed_pager, NULL);
    }

    thread->process = process;
    thread->tid = allocate_tid();
    thread->flags = THREAD_BLOCKED;
    arch_create_thread(&thread->arch, is_kernel_thread,
        start, arg, stack, stack_size);
    thread_list_append(&process->threads, thread);

    rq->thread = thread;
    runqueue_list_append(&runqueue, rq);

    DEBUG("kernel: created a thread #%d process=%p, start=%p (%s)",
        thread->tid, thread->process, start, is_kernel_thread ? "kernel" : "user");
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

void thread_switch_to(struct thread *next) {
    struct thread *current = CPUVAR->current_thread;
    CPUVAR->current_thread = next;
    CPUVAR->current_process = next->process;
INFO(">>> %d.%d", next->process->pid, next->tid);

    if (next->process != kernel_process) {
        arch_switch_vmspace(&next->process->vms.arch);
    }

    arch_switch(&current->arch, &next->arch);
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
    while (rq != NULL) {
        int state = thread_get_state(rq->thread);
        if (state == THREAD_RUNNABLE && rq->thread != CPUVAR->current_thread) {
            CPUVAR->current_runqueue = rq;
            thread_switch_to(rq->thread);
            return;
        }

        rq = rq->next;
    }

    rq = runqueue->next; // Skip idle thread.
    while (rq) {
        int state = thread_get_state(rq->thread);
        INFO("rq: %d state=%d", rq->thread->tid, state);
        if (state == THREAD_RUNNABLE && rq->thread != CPUVAR->current_thread) {
            CPUVAR->current_runqueue = rq;
            thread_switch_to(rq->thread);
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
    CPUVAR->current_process = runqueue->thread->process;
    CPUVAR->current_runqueue = runqueue;
    arch_switch(&current_thread->arch, &runqueue->thread->arch);

}


void thread_init(void) {

    runqueue_list_init(&runqueue);
    CPUVAR->current_runqueue = NULL;

    // Create an idle thread. We specify NULL as handler because it won't
    // be used.
    idle_thread = thread_create(kernel_process, (uptr_t) NULL, 0);
    CPUVAR->current_process = kernel_process;
    CPUVAR->current_thread = idle_thread;
    CPUVAR->current_runqueue = runqueue;
}
