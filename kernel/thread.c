#include "list.h"
#include "cpu.h" // FIXME
#include "memory.h"
#include "thread.h"
#include "process.h"

static tid_t last_tid = 1;


tid_t allocate_tid(void) {
    // FIXME
    return last_tid++;
}


struct thread *thread_create(struct process *process, uptr_t start, uptr_t arg) {
    bool is_kernel_thread = process == kernel_process;
    struct thread *thread = kmalloc(sizeof(*thread), KMALLOC_NORMAL);

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
    thread->resumed_count = 0;
    arch_create_thread(&thread->arch, is_kernel_thread,
        start, arg, stack, stack_size);
    thread_list_append(&process->threads, thread);

    DEBUG("kernel: created a thread #%d process=%p, start=%p (%s)",
        thread->tid, thread->process, start, is_kernel_thread ? "kernel" : "user");
    return thread;
}


void thread_destroy(struct thread *thread) {
    if (thread == CPUVAR->idle_thread) {
        PANIC("idle thread can't be destroyied");
    }

    // XXX: lock
    for (struct runqueue *rq = CPUVAR->runqueue; rq != NULL; rq = rq->next) {
        if (rq->thread == thread) {
            runqueue_list_remove(&CPUVAR->runqueue, rq);
            kfree(rq);
            break;
        }
    }

    thread_list_remove(&thread->process->threads, thread);
    arch_destroy_thread(&thread->arch);
    kfree(thread);
}


NORETURN void thread_destroy_current(void) {
    struct thread *thread = CPUVAR->current;
    if (thread == CPUVAR->idle_thread) {
        PANIC("idle thread can't be destroyied");
    }

    PANIC("%s: not yet implemented", __func__);
}


void thread_switch_to(struct thread *next) {
    struct thread *current = CPUVAR->current;
    CPUVAR->current = next;
INFO("[%d.%d] ==============================", next->process->pid, next->tid);

    if (next->process != kernel_process) {
        arch_switch_vmspace(&next->process->vms.arch);
    }

    arch_switch(&current->arch, &next->arch);
}


void thread_resume(struct thread *thread) {
    atomic_fetch_and_add(&thread->resumed_count, 1);
    INFO(">>> RESUME #%d by %d (%d)", thread->tid, CPUVAR->current->tid, thread->resumed_count);
    if (thread->resumed_count == 1) {
        thread->flags = (thread->flags & ~3) | THREAD_RUNNABLE;
        struct runqueue *rq = kmalloc(sizeof(*rq), KMALLOC_NORMAL);
        rq->thread = thread;
        runqueue_list_append(&CPUVAR->runqueue, rq);
    }
}


void thread_block(struct thread *thread) {
    INFO("BLK");

    atomic_fetch_and_sub(&thread->resumed_count, 1);
    INFO(">>> BLOCK  #%d by %d (%d)", thread->tid, CPUVAR->current->tid, thread->resumed_count);
    if (thread->resumed_count == 0) {
        thread->flags = (thread->flags & ~3) | THREAD_BLOCKED;
    }
}

void thread_block_current(void) {
    struct thread *current = CPUVAR->current;
    atomic_fetch_and_sub(&current->resumed_count, 1);
    INFO(">>> BLOCKC #%d (%d)", CPUVAR->current->tid, current->resumed_count);
    if (current->resumed_count <= 0) {
        current->flags = (current->flags & ~3) | THREAD_BLOCKED;
        thread_switch();
    }
}


void thread_switch(void) {
    // TODO: implement a fair and smart scheduler

    for (struct runqueue *rq = CPUVAR->runqueue; rq; rq = rq->next) {
        struct thread *next = rq->thread;

        if (next != CPUVAR->idle_thread && next != CPUVAR->current) {
            INFO("rq: %d state=%d from %d", rq->thread->tid, thread_get_state(rq->thread), CPUVAR->current->tid);
            if (thread_get_state(CPUVAR->current) == THREAD_RUNNABLE) {
                INFO("readd: %p", CPUVAR->current->tid);
                struct runqueue *current_rq = kmalloc(sizeof(*rq), KMALLOC_NORMAL);
                current_rq->thread = CPUVAR->current;
                runqueue_list_append(&CPUVAR->runqueue, current_rq);
            }

            runqueue_list_remove(&CPUVAR->runqueue, rq);
            for (struct runqueue *rq1 = CPUVAR->runqueue; rq1; rq1 = rq1->next) {
                INFO("rq1: %d", rq1->thread->tid);
            }

            thread_switch_to(next);
            return;
        }
    }

    if (thread_get_state(CPUVAR->current) == THREAD_RUNNABLE) {
        // No other thread to run. Resume the current thread.
        return;
    }

    // No threads are runnable. Resume the idle thread. Assuming `runqueue` points
    // to the idle thread.
    struct thread *current_thread = CPUVAR->current;
    CPUVAR->current = CPUVAR->idle_thread;
    arch_switch(&current_thread->arch, &CPUVAR->idle_thread->arch);
}


void thread_init(void) {
     runqueue_list_init(&CPUVAR->runqueue);

    // Create an idle thread. We specify NULL as start address because it won't
    // be used.
    CPUVAR->idle_thread = thread_create(kernel_process, (uptr_t) NULL, 0);
    CPUVAR->current = CPUVAR->idle_thread;
}
