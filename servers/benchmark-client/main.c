#include <resea.h>
#include <resea/benchmark.h>
#include <resea/discovery.h>
#include <resea/logging.h>

static inline u64_t rdtscp(u32_t *cpu) {
    u64_t rax, rdx;

    __asm__ __volatile__(
        "rdtscp"
    : "=a"(rax), "=d"(rdx), "=c"(*cpu));

    return (rdx << 32) | rax;
}


void main(void) {
//    printf("A\n");

    channel_t server;
    call_discovery_connect(1, BENCHMARK_SERVICE, &server);

    payload_t a0 = 0xabcdef000000004;
    payload_t a1 = 0xabcdef000000001;
    payload_t a2 = 0xabcdef000000002;
    payload_t a3 = 0xabcdef000000003;

    for (int i = 0; i < 1000; i++) {
        payload_t r;
        u32_t cpu_start, cpu_end;

        u64_t start = rdtscp(&cpu_start);
//        ipc_call(server, 0, a0, a1, a2, a3, &r, &r, &r, &r);
        u64_t cycles = rdtscp(&cpu_end) - start;
        printf(">>>>> %d, %d, %d", cycles, cpu_start, cpu_end);
    }
}
