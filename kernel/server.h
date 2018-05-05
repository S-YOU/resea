#ifndef __SERVER_H__
#define __SERVER_H__

extern struct channel *kernel_channel;

void kernel_server(void);
void kernel_server_init(void);

#endif
