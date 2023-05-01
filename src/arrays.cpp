#include <benchmark/benchmark.h>

#include "utils.h"

#define ANV_ARR_IMPLEMENTATION
#include "anv_arr.h"

#include "kvec.h"

#define STBDS_NO_SHORT_NAMES
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define NO_STRETCHY_BUFFER_SHORT_NAMES
#include "stretchy_buffer.h"

typedef struct item_t {
    int a;
} item_t;

static size_t anv_reallocator(size_t old_capacity)
{
    // same as kvec
    return old_capacity << 1;
}

static void BM_arr_anv_arr_push(benchmark::State& state)
{
    item_t* arr = (item_t*)anv_arr_new(2, sizeof(item_t));
    anv_arr_config_reallocator_fn(anv_reallocator);
    ensure(arr);
    for (auto _ : state) {
        item_t it = { 10 };
        anv_arr__push((anv_arr_t*)&arr, &it);
    }
    ensure(anv_arr_length(arr) == state.max_iterations);
    anv_arr_destroy(arr);
}
BENCHMARK(BM_arr_anv_arr_push);

static void BM_arr_kvec_push(benchmark::State& state)
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
BENCHMARK(BM_arr_kvec_push);

static void BM_arr_stb_ds_push(benchmark::State& state)
{
    item_t* arr = NULL;
    for (auto _ : state) {
        item_t it = { 10 };
        stbds_arrput(arr, it);
    }
    ensure(stbds_arrlen(arr) == state.max_iterations);
    stbds_arrfree(arr);
}
BENCHMARK(BM_arr_stb_ds_push);

static void BM_arr_stb_stretchy_push(benchmark::State& state)
{
    item_t* arr = NULL;
    for (auto _ : state) {
        item_t it = { 10 };
        stb_sb_push(arr, it);
    }
    ensure(stb_sb_count(arr) == state.max_iterations);
    stb_sb_free(arr);
}
BENCHMARK(BM_arr_stb_stretchy_push)->Iterations(50000000);

static void BM_arr_anv_arr_push_preallocated(benchmark::State& state)
{
    item_t* arr = (item_t*)anv_arr_new(state.max_iterations, sizeof(item_t));
    ensure(arr);
    for (auto _ : state) {
        item_t it = { 10 };
        anv_arr__push((anv_arr_t*)&arr, &it);
    }
    ensure(anv_arr_length(arr) == state.max_iterations);
    anv_arr_destroy(arr);
}
BENCHMARK(BM_arr_anv_arr_push_preallocated);

static void BM_arr_kvec_push_preallocated(benchmark::State& state)
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
BENCHMARK(BM_arr_kvec_push_preallocated);

static void BM_arr_stb_ds_push_preallocated(benchmark::State& state)
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
BENCHMARK(BM_arr_stb_ds_push_preallocated);

static void BM_arr_stb_stretchy_push_preallocated(benchmark::State& state)
{
    item_t* arr = NULL;
    stb__sbgrow(arr, state.max_iterations);
    for (auto _ : state) {
        item_t it = { 10 };
        stb_sb_push(arr, it);
    }
    ensure(stb_sb_count(arr) == state.max_iterations);
    stb_sb_free(arr);
}
BENCHMARK(BM_arr_stb_stretchy_push_preallocated)->Iterations(50000000);
