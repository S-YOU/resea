#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "memory.h"
#include "thread.h"

struct process {
    tid_t pid;
    struct thread *threads;
    struct vmspace vms;
};

extern struct process *kernel_process;

struct process *process_create(void);
void process_destroy(struct process *process);
void process_init(void);

#endif
