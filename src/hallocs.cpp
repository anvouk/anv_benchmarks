#include <benchmark/benchmark.h>

extern "C" {
#define HALLOC_IMPLEMENTATION
#include "halloc.h"

#define ANV_HALLOC_IMPLEMENTATION
#include "anv_halloc.h"
}

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
        free(root);
    }
}
BENCHMARK(BM_h_malloc_new_free);

static void BM_h_halloc_new_free_root(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)h_malloc(sizeof(test_t)));
        h_free(root);
    }
}
BENCHMARK(BM_h_halloc_new_free_root);

static void BM_h_anv_halloc_new_free_root(benchmark::State& state)
{
    test_t* root = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(root = (test_t*)anvh_alloc(NULL, sizeof(test_t), 0));
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_new_free_root);

static void BM_h_halloc_new_children(benchmark::State& state)
{
    test_t* root = (test_t*)h_malloc(sizeof(test_t));
    test_t *child = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(child = (test_t*)h_malloc(sizeof(test_t)));
        hattach(child, root);
    }
    h_free(root);
}
BENCHMARK(BM_h_halloc_new_children);

static void BM_h_anv_halloc_new_children(benchmark::State& state)
{
    test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.iterations());
    test_t *child = NULL;
    for (auto _ : state) {
        benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), state.range(0)));
    }
    anvh_free(root);
}
BENCHMARK(BM_h_anv_halloc_new_children)
    ->Arg(0)->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

static void BM_h_halloc_alloc_free_with_children(benchmark::State& state)
{
    test_t *child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)h_malloc(sizeof(test_t));
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)h_malloc(sizeof(test_t)));
            hattach(child, root);
        }
        h_free(root);
    }
}
BENCHMARK(BM_h_halloc_alloc_free_with_children)
    ->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

static void BM_h_anv_halloc_alloc_free_with_children(benchmark::State& state)
{
    test_t *child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.iterations());
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), 0));
        }
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_alloc_free_with_children)
    ->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);

static void BM_h_anv_halloc_alloc_free_with_children_children(benchmark::State& state)
{
    test_t *child = NULL;
    for (auto _ : state) {
        test_t* root = (test_t*)anvh_alloc(NULL, sizeof(test_t), state.iterations());
        for (size_t i = 0; i < state.range(0); ++i) {
            benchmark::DoNotOptimize(child = (test_t*)anvh_alloc(root, sizeof(test_t), state.range(0)));
        }
        anvh_free(root);
    }
}
BENCHMARK(BM_h_anv_halloc_alloc_free_with_children_children)
    ->Arg(8)->Arg(64)->Arg(512)->Arg(4096)->Arg(8192);
