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


#define TEST_NUM 128
u64_t results[TEST_NUM];

static inline void bench_start(int i) {
    u32_t cpu;
    results[i] = rdtscp(&cpu);
}

static inline void bench_end(int i) {
    u32_t cpu;
    results[i] = rdtscp(&cpu) - results[i];
}

void bench_summarize(void) {
    u64_t min = 0xffffffffffffffff, max = 0, mean = 0;
    for (int i = 0; i < TEST_NUM; i++) {
        u64_t r = results[i];

        if (r < min)
            min = r;
        if (r > max)
            max = r;

        mean += r;
    }

    // Sort the results in order to obtain these parameters.
    u64_t median, q1, q3;
    bool swapped = false;
    while (swapped) {
        swapped = false;
        for (int i = 0; i < TEST_NUM - 1; i++) {
            if (results[i] > results[i + 1]) {
                u64_t tmp = results[i + 1];
                results[i + 1] = results[i];
                results[i] = tmp;
                swapped = true;
            }
        }
    }

    q1 = results[TEST_NUM / 4];
    q3 = results[(TEST_NUM / 4) * 3];
    median = results[TEST_NUM / 2];
    mean /= TEST_NUM;

    printf("min:  %d\t1Q: %d\tmedian: %d\t",
        min, q1, median);
    printf("mean: %d\t3Q: %d\tmax:    %d\t",
        mean, q3, max);
}

void rdtscp_latency_benchmark(void) {
    printf("The latency of bench_start/bench_end\n");
    printf("-----------------------------------------\n");
    for (int i = 0; i < TEST_NUM; i++) {
        bench_start(i);
        bench_end(i);
    }

    bench_summarize();
}

void rpc_latency_benchmark(void) {
    channel_t server;
    call_discovery_connect(1, BENCHMARK_SERVICE, &server);

    payload_t a0 = 0xabcdef000000004;
    payload_t a1 = 0xabcdef000000001;
    payload_t a2 = 0xabcdef000000002;
    payload_t a3 = 0xabcdef000000003;

    printf("The latency of RPC style IPC (round-trip)\n");
    printf("-----------------------------------------\n");
    for (int i = 0; i < TEST_NUM; i++) {
        payload_t r;
        bench_start(i);
        ipc_call(server, 0, a0, a1, a2, a3, &r, &r, &r, &r);
        bench_end(i);
    }

    bench_summarize();
}

void main(void) {
    printf("Benchmarks are being started. It’s time to brew coffee!\n\n");
    rdtscp_latency_benchmark();
    rpc_latency_benchmark();
    printf("Finished all benchmarks.");
}
