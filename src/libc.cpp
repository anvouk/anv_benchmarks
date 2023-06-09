#include <benchmark/benchmark.h>

#include <stdlib.h>
#include <string.h>

#include "utils.h"

// Note: these are to determine a baseline reference for other benchmarks
// which use the following memory functions underneath. The goal is NOT to
// benchmark all functions possible.

static void BM_libc_memmove(benchmark::State& state)
{
    char* a = (char*)malloc(state.range(0));
    char* b = (char*)malloc(state.range(0));
    memset(b, 'x', state.range(0));
    for (auto _ : state) {
        memmove(a, b, state.range(0));
    }
    free(a);
    free(b);
}
BENCHMARK(BM_libc_memmove)->Arg(8)->Arg(64)->Arg(4096);

static void BM_libc_memcpy(benchmark::State& state)
{
    char* a = (char*)malloc(state.range(0));
    char* b = (char*)malloc(state.range(0));
    memset(b, 'x', state.range(0));
    for (auto _ : state) {
        memcpy(a, b, state.range(0));
    }
    free(a);
    free(b);
}
BENCHMARK(BM_libc_memcpy)->Arg(8)->Arg(64)->Arg(4096);

static void BM_libc_malloc_and_free(benchmark::State& state)
{
    void* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = malloc(state.range(0)));
        ensure(root);
        free(root);
    }
}
BENCHMARK(BM_libc_malloc_and_free)
    ->Arg(1)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(128)->Arg(512)->Arg(2048)->Arg(4096);
