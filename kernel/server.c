#include "types.h"
#include "ipc.h"
#include <resea/exit.h>
#include <resea/logging.h>
#include <resea/discovery.h>
#include "thread.h"
#include "process.h"
#include "ipc.h"
#include "server.h"

struct channel *kernel_channel;
struct service *services;
struct client *clients;


static inline void handle_exit_exit(channel_t from, u32_t error) {
    /* TODO */
}


static inline error_t handle_logging_emit(channel_t from, string_t str, usize_t length) {

    for (usize_t i = 0; i < length; i++) {
        arch_putchar(str[0]);
    }

    arch_putchar('\n');
    return ERROR_NONE;
}


static inline error_t handle_discovery_register(channel_t from, u32_t service_type, channel_t server) {
    DEBUG("discovery.register: service=%d (%d)", service_type, server);

    struct service *service = kmalloc(sizeof(*service), KMALLOC_NORMAL);
    service->service_type = service_type;
    service->server = server;
    service_list_append(&services, service);

    /* Connect pending clients. */
    for (struct client *c = clients; c != NULL;) {
        if (c->service_type == service_type) {
            channel_t client = ipc_connect(service->server);
            INFO("client %d -> c->ch+%d <<<<<<<<<,,", client, c->ch);
            ipc_send(c->ch, DISCOVERY_CONNECT_REPLY_HEADER, client, 0, 0, 0);
        }

        struct client *next = c->next;
        client_list_remove(&clients, c);
        c = next;
    }

    return ERROR_NONE;
}


static inline error_t handle_discovery_connect(channel_t from, u32_t service_type, channel_t *client) {
    DEBUG("discovery.connect: service=%d", service_type);

    for (struct service *service = services; service != NULL; service = service->next) {
        if (service->service_type == service_type) {
            *client = ipc_connect(service->server);
            return ERROR_NONE;
        }
    }

    struct client *c = kmalloc(sizeof(*client), KMALLOC_NORMAL);
    c->service_type = service_type;
    c->ch = from;
    client_list_append(&clients, c);
    return ERROR_DONT_REPLY;
}


void kernel_server_mainloop(channel_t server) {
    channel_t from;
    payload_t a0, a1, a2, a3;
    payload_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;
    header_t header = ipc_recv(server, &from, &a0, &a1, &a2, &a3);
    for (;;) {
        error_t error = 0;
        switch (MSGTYPE(header)) {
            case EXIT_EXIT_MSG:
                handle_exit_exit(from, (error_t) a0);
                /* The caller thread is terminated. Needless to reply. */
                continue;
            case LOGGING_EMIT_MSG:
                error = handle_logging_emit(from, (string_t) a0, (usize_t) a1);
                header = LOGGING_EMIT_REPLY_MSG | (error << ERROR_OFFSET);
                continue;
            case DISCOVERY_REGISTER_MSG:
                error = handle_discovery_register(from, (u32_t) a0, (channel_t) a1);
                header = DISCOVERY_REGISTER_REPLY_MSG | (error << ERROR_OFFSET);
                break;
            case DISCOVERY_CONNECT_MSG:
                error = handle_discovery_connect(from, (u32_t) a0, (channel_t *) &r0);
                header = DISCOVERY_CONNECT_REPLY_MSG | (error << ERROR_OFFSET);
                break;

            default:
                /* Unknown message. */
                header = ERROR_UNKNOWN_MSG << ERROR_OFFSET;
                DEBUG("kernel: unknown message %p", MSGTYPE(header));
                break;
        }

        if (error == ERROR_DONT_REPLY) {
            INFO("recved %p", header);
            header = ipc_recv(server, &from, &a0, &a1, &a2, &a3);
        } else {
            INFO("replyed %p", header);
            header = ipc_replyrecv(&from, header, r0, r1, r2, r3, &a0, &a1, &a2, &a3);
        }
    }
}


void kernel_server(void) {
    kernel_server_mainloop(kernel_channel->cid);
}


void kernel_server_init(void) {
    service_list_init(&services);
    client_list_init(&clients);

    kernel_channel = channel_create(kernel_process);
    thread_set_state(
        thread_create(kernel_process, (uptr_t) kernel_server, 0),
        THREAD_RUNNABLE
    );
}
