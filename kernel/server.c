#include <resea.h>
#include <resea/discovery.h>
#include "thread.h"
#include "process.h"
#include "ipc.h"
#include "server.h"


static struct channel *kernel_channel;


static inline error_t handle_discovery_register(channel_t from, string_t name, usize_t name_length, channel_t server) {
    /* TODO */
    return ERROR_NONE;
}


static inline error_t handle_discovery_connect(channel_t from, string_t name, usize_t name_length, channel_t *client) {
    /* TODO */
    return ERROR_NONE;
}


void kernel_server_mainloop(channel_t server) {
    channel_t from;
    payload_t a0, a1, a2, a3;
    payload_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;
    header_t header = ipc_recv(server, &from, &a0, &a1, &a2, &a3);
    for (;;) {
        error_t error;
        switch (MSGTYPE(header)) {
            case DISCOVERY_REGISTER_MSG:
                error = handle_discovery_register(from, (string_t) a0, (usize_t) a1, (channel_t) a2);
                header = DISCOVERY_REGISTER_REPLY_MSG | error;
                break;
            case DISCOVERY_CONNECT_MSG:
                error = handle_discovery_connect(from, (string_t) a0, (usize_t) a1, (channel_t *) &r0);
                header = DISCOVERY_CONNECT_REPLY_MSG | error;
                break;

            default:
                /* Unknown message. */
                break;
        }

        ipc_replyrecv(server, &from, header, r0, r1, r2, r3, &a0, &a1, &a2, &a3);
    }
}


void kernel_server(void) {
    kernel_server_mainloop(kernel_channel->cid);
}


void kernel_server_init(void) {
    kernel_channel = channel_create(kernel_process);
    thread_set_state(
        thread_create(kernel_process, (uintptr_t) kernel_server, 0),
        THREAD_RUNNABLE
    );
}
