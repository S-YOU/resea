#ifndef __IPC_H__
#define __IPC_H__

#include <resea/types.h>

void handle_syscall(uint64_t type, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);

#endif
