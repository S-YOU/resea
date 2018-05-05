#ifndef __RESEA_STUB_kernel_H__
#define __RESEA_STUB_kernel_H__

#include <resea.h>


#define EXIT_EXIT_MSG       (2)
#define EXIT_EXIT_REPLY_MSG (3)
#define EXIT_EXIT_HEADER ((EXIT_EXIT_MSG << 32) | (0 | (0 << 8 + (0 * 3))))
#define EXIT_EXIT_HEADER ((EXIT_EXIT_MSG << 32) | (0))
static inline header_t call_exit_exit(channel_t __server, error_t exit_code) {
    payload_t __unused;

    return ipc_call(
        __server, EXIT_EXIT_HEADER, (payload_t) exit_code, 0, 0, 0, &__unused, &__unused, &__unused, &__unused
    );
}

void kernel_server_mainloop(void) {
    header_t header;
    channel_t from;
    header = ipc_recv(server, &from, &a0, &a1, &a2, &a3);
    for (;;) {
        switch (MSGTYPE(header)) {
            case EXIT_EXIT_MSG:
                error_t error = handle_exit_exit(__reply_to, (error_t) a0);
                header = EXIT_EXIT_REPLY_MSG | error;
                break;
            default:
                /* UNknown Unknown message. */
                break;
        }

        ipc_replyrecv(server, &from, header, r0, r1, r2, r3, &a0, &a1, &a2, &a3);
    }
}


#endif

