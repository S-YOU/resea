#include <resea.h>


void main(void) {
    channel_t server = ipc_open();
    ipc_call(1, DISCOVERY_REGISTER, BENCHMARK_SERVICE, server, 0, 0, 0, 0, 0, 0);

    payload_t r0 = 0xabcdef000000004;
    payload_t r1 = 0xabcdef000000001;
    payload_t r2 = 0xabcdef000000002;
    payload_t r3 = 0xabcdef000000003;

    payload_t a0, a1, a2, a3;
    channel_t from;
    ipc_recv(server, &from, &a0, &a1, &a2, &a3);
    for (;;) {
        ipc_replyrecv(server, &from, BENCHMARK_REPLY, r0, r1, r2, r3, &a0, &a1, &a2, &a3);
    }
}
