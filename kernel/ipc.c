#include "cpu.h"
#include "thread.h"
#include "process.h"
#include "ipc.h"


static inline struct channel *get_channel_by_id(channel_t ch) {
    size_t channels_max = CPUVAR->current_process->channels_max;
    struct channel **channels = (struct channel **) &CPUVAR->current_process->channels;

    return (ch > channels_max) ? NULL : channels[ch - 1];
}

channel_t ipc_open(void) {
    struct process *process = CPUVAR->current_process;

    kmutex_state_t state = kmutex_lock_irq_disabled(&process->lock);
    size_t channels_max = process->channels_max;
    for (size_t i = 0; i < channels_max; i++) {
        struct channel *ch = &process->channels[i];
        if (ch->flags == 0) {
            DEBUG("ipc_open: #%d ch=%d", process->pid, i + 1);
            ch->flags = 1;
            ch->linked_to = NULL;
            ch->transfer_to = NULL;
            ch->receiver = NULL;
            ch->sender = NULL;
            kmutex_unlock_restore_irq(&process->lock, state);
            return i + 1;
        }
    }

    DEBUG("ipc_open: failed to allocate #%d", process->pid);
    kmutex_unlock_restore_irq(&process->lock, state);
    return 0;
}


type_t invalid_syscall(void) {
    INFO("invalid syscall");
    return 0;
}
