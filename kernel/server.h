#ifndef __SERVER_H__
#define __SERVER_H__

#include <list.h>

struct service {
    struct service *next;
    u32_t service_type;
    channel_t server;
};

DEFINE_LIST(service, struct service)

extern struct channel *kernel_channel;

void kernel_server(void);
void kernel_server_init(void);

#endif
