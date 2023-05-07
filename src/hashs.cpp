#include <benchmark/benchmark.h>

#include <time.h>

#include "utils.h"

#include "MurmurHash2.h"
#include "MurmurHash3.h"
#include "fasthash.h"
#include "stb_ds_hash.h"

// these are only 32 bytes

static void BM_hashs_stbds_hash_bytes_32(benchmark::State& state)
{
    size_t sz = 32;
    void* chunk = malloc(sz);
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_hash_bytes(chunk, sz, seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_hash_bytes_32);

static void BM_hashs_stbds_hash_bytes_accidental_32(benchmark::State& state)
{
    size_t sz = 32;
    void* chunk = malloc(sz);
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_hash_bytes_accidental(chunk, sz, seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_hash_bytes_accidental_32);

static void BM_hashs_stbds_hash_bytes_murmur_32(benchmark::State& state)
{
    size_t sz = 32;
    void* chunk = malloc(sz);
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_hash_bytes_murmur(chunk, sz, seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_hash_bytes_murmur_32);

static void BM_hashs_stbds_hash_bytes_64(benchmark::State& state)
{
    size_t sz = 64;
    void* chunk = malloc(sz);
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_hash_bytes_64(chunk, sz, seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_hash_bytes_64);

// more generic hashes

static void BM_hashs_stbds_siphash_bytes_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_siphash_bytes(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_siphash_bytes_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_stbds_siphash_2_4_bytes_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(stbds_siphash_2_4_bytes(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_stbds_siphash_2_4_bytes_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_fasthash_32_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(fasthash32(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_fasthash_32_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_murmur2_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(MurmurHash2A(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_murmur2_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_murmur3_chunk(benchmark::State& state)
{
    void* out = malloc(32);
    ensure(out);
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        MurmurHash3_x86_32(chunk, state.range(0), seed, out);
    }
    free(chunk);
    free(out);
}
BENCHMARK(BM_hashs_murmur3_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_fasthash_64_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(fasthash64(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_fasthash_64_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_murmur2_64_chunk(benchmark::State& state)
{
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        benchmark::DoNotOptimize(MurmurHash64A(chunk, state.range(0), seed));
    }
    free(chunk);
}
BENCHMARK(BM_hashs_murmur2_64_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);

static void BM_hashs_murmur3_64_chunk(benchmark::State& state)
{
    void* out = malloc(128);
    ensure(out);
    void* chunk = malloc(state.range(0));
    ensure(chunk);
    size_t seed = time(NULL);
    for (auto _ : state) {
        MurmurHash3_x64_128(chunk, state.range(0), seed, out);
    }
    free(chunk);
    free(out);
}
BENCHMARK(BM_hashs_murmur3_64_chunk)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(64)->Arg(256);
