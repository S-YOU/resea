#include <list.h>
#include "memory.h"
#include "process.h"

struct process *process_create(void) {
    struct process *process = kmalloc(sizeof(*process), KMALLOC_NORMAL);
    process->pid = allocate_tid();
    thread_list_init(&process->threads);
    return process;
}

void process_destroy(UNUSED struct process *process) {

}

struct process *kernel_process = NULL;
void process_init(void) {
    kernel_process = process_create();
}
