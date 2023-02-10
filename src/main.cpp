#include <benchmark/benchmark.h>

#define ANV_METALLOC_IMPLEMENTATION
#define ANV_ARR_IMPLEMENTATION
#include "anv_arr.h"

#include "kvec.h"

#define STBDS_NO_SHORT_NAMES
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#undef ensure
#define ensure(x)                                                                                                      \
    do {                                                                                                               \
        if (!(x)) {                                                                                                    \
            fprintf(stderr, "@@@ Assert failed: '" #x "' @@@");                                                        \
        }                                                                                                              \
    } while (0)

typedef struct item_t {
    int a;
} item_t;

static size_t anv_reallocator(size_t old_capacity)
{
    // same as kvec
    return old_capacity << 1;
}

static void BM_anv_arr_push_preallocated(benchmark::State& state)
{
    anv_arr_t arr = anv_arr_new(state.max_iterations, sizeof(item_t));
    ensure(arr);
    for (auto _ : state) {
        item_t it = { 10 };
        anv_arr__push(&arr, &it);
    }
    ensure(anv_arr_length(arr) == state.max_iterations);
    anv_arr_destroy(arr);
}
BENCHMARK(BM_anv_arr_push_preallocated)->Iterations(1000000000);

static void BM_anv_arr_push(benchmark::State& state)
{
    anv_arr_t arr = anv_arr_new(2, sizeof(item_t));
    anv_arr_config_reallocator_fn(anv_reallocator);
    ensure(arr);
    for (auto _ : state) {
        item_t it = { 10 };
        anv_arr__push(&arr, &it);
    }
    ensure(anv_arr_length(arr) == state.max_iterations);
    anv_arr_destroy(arr);
}
BENCHMARK(BM_anv_arr_push)->Iterations(1000000000);

static void BM_kvec_push_preallocated(benchmark::State& state)
{
    kvec_t(item_t) array;
    kv_init(array);
    kv_resize(item_t, array, state.max_iterations);
    for (auto _ : state) {
        item_t it = { 10 };
        kv_push(item_t, array, it);
    }
    ensure(array.n == state.max_iterations);
    kv_destroy(array);
}
BENCHMARK(BM_kvec_push_preallocated)->Iterations(1000000000);

static void BM_kvec_push(benchmark::State& state)
{
    kvec_t(item_t) array;
    kv_init(array);
    for (auto _ : state) {
        item_t it = { 10 };
        kv_push(item_t, array, it);
    }
    ensure(array.n == state.max_iterations);
    kv_destroy(array);
}
BENCHMARK(BM_kvec_push)->Iterations(1000000000);

static void BM_stb_ds_push_preallocated(benchmark::State& state)
{
    item_t* arr = NULL;
    stbds_arrsetcap(arr, state.max_iterations);
    ensure(arr);
    for (auto _ : state) {
        item_t it = { 10 };
        stbds_arrput(arr, it);
    }
    ensure(stbds_arrlen(arr) == state.max_iterations);
    stbds_arrfree(arr);
}
BENCHMARK(BM_stb_ds_push_preallocated)->Iterations(1000000000);

static void BM_stb_ds_push(benchmark::State& state)
{
    item_t* arr = NULL;
    for (auto _ : state) {
        item_t it = { 10 };
        stbds_arrput(arr, it);
    }
    ensure(stbds_arrlen(arr) == state.max_iterations);
    stbds_arrfree(arr);
}
BENCHMARK(BM_stb_ds_push)->Iterations(1000000000);

BENCHMARK_MAIN();
