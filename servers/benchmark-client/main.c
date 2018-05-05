#include <resea.h>
#include <resea/benchmark.h>
#include <resea/discovery.h>

void main(void) {
    channel_t server;
    call_discovery_connect(1, BENCHMARK_SERVICE, &server);
    __asm__ ("xchg %bx,%bx");

    payload_t a0 = 0xabcdef000000004;
    payload_t a1 = 0xabcdef000000001;
    payload_t a2 = 0xabcdef000000002;
    payload_t a3 = 0xabcdef000000003;

    for (;;) {
        payload_t r;
        ipc_call(server, 0, a0, a1, a2, a3, &r, &r, &r, &r);
    }
}
