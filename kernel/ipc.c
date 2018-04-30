#include "ipc.h"


void handle_syscall(uint64_t type, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5) {
    INFO("syscall: %p %d %d %d %d %d", type, a1, a2, a3, a4, a5);
}
