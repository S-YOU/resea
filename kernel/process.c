#include <list.h>
#include "memory.h"
#include "process.h"


struct process *process_create(void) {
    struct process *process = kmalloc(sizeof(*process), KMALLOC_NORMAL);

    process->pid = allocate_tid();
    thread_list_init(&process->threads);
    memory_create_vmspace(&process->vms);

    return process;
}


void process_destroy(UNUSED struct process *process) {

    PANIC("%s: not yet implemented", __func__);
}


struct process *kernel_process = NULL;
void process_init(void) {
    kernel_process = process_create();
}
