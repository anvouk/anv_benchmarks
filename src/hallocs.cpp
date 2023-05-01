#include <benchmark/benchmark.h>

#define HALLOC_IMPLEMENTATION
#include "halloc.h"

#define ANV_HALLOC_IMPLEMENTATION
#include "anv_halloc.h"

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

static void BM_h_anv_halloc_new_free_root(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)anvh_alloc(NULL, sizeof(test_t), 0));
        ensure(root);
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_new_free_root);

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

static void BM_h_anv_halloc_new_children(benchmark::State& state)
{
    test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.max_iterations);
    ensure(root);
    test_t* child = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), state.range(0)));
        ensure(child);
    }
    anvh_free(root);
}
BENCHMARK(BM_h_anv_halloc_new_children)->Arg(0)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

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

static void BM_h_anv_halloc_alloc_free_with_children(benchmark::State& state)
{
    test_t* child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.max_iterations);
        ensure(root);
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), 0));
            ensure(child);
        }
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_alloc_free_with_children)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

static void BM_h_anv_halloc_alloc_free_with_children_children(benchmark::State& state)
{
    test_t* child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.max_iterations);
        ensure(root);
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), state.range(0)));
            ensure(child);
        }
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_alloc_free_with_children_children)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);
