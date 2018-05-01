#include <resea.h>


void main(void) {
    channel_t server = ipc_open();
    ipc_call(1, DISCOVERY_REGISTER, BENCHMARK_SERVICE, server, 0, 0, 0, 0, 0, 0);

    payload_t r1 = 0xabcdef000000001;
    payload_t r2 = 0xabcdef000000002;
    payload_t r3 = 0xabcdef000000003;
    payload_t r4 = 0xabcdef000000004;

    channel_t from;
    ipc_recv(server, &from, &a1, &a2, &a3, &a4);
    for (;;) {
        ipc_replyrecv(from, server, r1, r2, r3, r4, &a1, &a2, &a3, &a4);
    }
}
