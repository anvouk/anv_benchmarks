#include <benchmark/benchmark.h>

#define HALLOC_IMPLEMENTATION
#include "halloc.h"

#define STB_ALLOC_IMPLEMENTATION
#include "stb_alloc.h"

#undef ensure
#define ensure(x)                                                                           \
    do {                                                                                    \
        if (!(x)) {                                                                         \
            fprintf(stderr, "@@@ [%s:%d] Assert failed: '%s' @@@", __FILE__, __LINE__, #x); \
            exit(1);                                                                        \
        }                                                                                   \
    } while (0)

typedef struct test_t {
    int a;
    int b;
    const char* str;
    uint64_t t;
} test_t;

static void BM_h_malloc_new_free(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)malloc(sizeof(test_t)));
        ensure(root);
        free(root);
    }
}
BENCHMARK(BM_h_malloc_new_free);

static void BM_h_halloc_new_free_root(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)h_malloc(sizeof(test_t)));
        ensure(root);
        h_free(root);
    }
}
BENCHMARK(BM_h_halloc_new_free_root);

static void BM_h_stb_new_free_root(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)stb_malloc(NULL, sizeof(test_t)));
        ensure(root);
        stb_free(root);
    }
}
BENCHMARK(BM_h_stb_new_free_root);

static void BM_h_halloc_new_children(benchmark::State& state)
{
    test_t* root = (test_t*)h_malloc(sizeof(test_t));
    ensure(root);
    test_t* child = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(child = (test_t*)h_malloc(sizeof(test_t)));
        ensure(child);
        hattach(child, root);
    }
    h_free(root);
}
BENCHMARK(BM_h_halloc_new_children);

static void BM_h_stb_new_children(benchmark::State& state)
{
    test_t* root = (test_t*)stb_malloc(NULL, sizeof(test_t));
    ensure(root);
    test_t* child = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(child = (test_t*)stb_malloc(root, sizeof(test_t)));
        ensure(child);
    }
    stb_free(root);
}
BENCHMARK(BM_h_stb_new_children);

static void BM_h_halloc_alloc_free_with_children(benchmark::State& state)
{
    test_t* child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)h_malloc(sizeof(test_t));
        ensure(root);
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)h_malloc(sizeof(test_t)));
            ensure(child);
            hattach(child, root);
        }
        h_free(root);
    }
}
BENCHMARK(BM_h_halloc_alloc_free_with_children)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

static void BM_h_stb_alloc_free_with_children(benchmark::State& state)
{
    test_t* child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)stb_malloc(NULL, sizeof(test_t));
        ensure(root);
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)stb_malloc(root, sizeof(test_t)));
            ensure(child);
        }
        stb_free(root);
    }
}
BENCHMARK(BM_h_stb_alloc_free_with_children)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);
