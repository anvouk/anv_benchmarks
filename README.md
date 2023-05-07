# anv_benchmarks

benchmarks about random stuff I needed at the moment. There's a bit of everything.

## Benchmark results

Run on my workstation:
```
CPU Xeon E5-2699 v3
RAM 2133MHz ECC
SSD
```

Results:
```
Run on (36 X 3600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x18)
  L1 Instruction 32 KiB (x18)
  L2 Unified 256 KiB (x18)
  L3 Unified 46080 KiB (x1)
Load Average: 0.95, 0.69, 0.50
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------------------
Benchmark                                                          Time             CPU   Iterations
----------------------------------------------------------------------------------------------------
BM_libc_memmove/8                                               2.30 ns         2.30 ns    305865876
BM_libc_memmove/64                                              2.01 ns         2.01 ns    351023766
BM_libc_memmove/4096                                            51.5 ns         51.5 ns     13468984
BM_libc_memcpy/8                                                2.59 ns         2.59 ns    272072177
BM_libc_memcpy/64                                               2.30 ns         2.30 ns    307727125
BM_libc_memcpy/4096                                             50.7 ns         50.7 ns     13625703
BM_libc_malloc_and_free/1                                       10.6 ns         10.6 ns     65523947
BM_libc_malloc_and_free/8                                       10.6 ns         10.6 ns     66609989
BM_libc_malloc_and_free/16                                      11.5 ns         11.5 ns     60918143
BM_libc_malloc_and_free/32                                      11.5 ns         11.5 ns     61400371
BM_libc_malloc_and_free/64                                      11.5 ns         11.5 ns     60707393
BM_libc_malloc_and_free/128                                     11.5 ns         11.5 ns     59762447
BM_libc_malloc_and_free/512                                     11.5 ns         11.5 ns     61173679
BM_libc_malloc_and_free/2048                                    34.0 ns         34.0 ns     20597330
BM_libc_malloc_and_free/4096                                    38.5 ns         38.5 ns     18196139
BM_arr_anv_arr_push                                             4.30 ns         4.30 ns    164012063
BM_arr_kvec_push                                               0.833 ns        0.833 ns    810187558
BM_arr_stb_ds_push                                              2.31 ns         2.31 ns    298924491
BM_arr_stb_stretchy_push/iterations:50000000                    2.30 ns         2.30 ns     50000000
BM_arr_anv_arr_push_preallocated                                3.94 ns         3.94 ns    178588350
BM_arr_kvec_push_preallocated                                   1.07 ns         1.07 ns    651092430
BM_arr_stb_ds_push_preallocated                                 1.34 ns         1.34 ns    504798568
BM_arr_stb_stretchy_push_preallocated/iterations:50000000       2.25 ns         2.25 ns     50000000
BM_h_halloc_new_free_root                                       15.5 ns         15.5 ns     43701019
BM_h_stb_new_free_root                                          14.0 ns         14.0 ns     50096284
BM_h_halloc_new_children                                        22.5 ns         22.5 ns     26631089
BM_h_stb_new_children                                           25.9 ns         25.9 ns     54809475
BM_h_halloc_alloc_free_with_children/8                           206 ns          206 ns      3348285
BM_h_halloc_alloc_free_with_children/64                         1557 ns         1557 ns       444687
BM_h_halloc_alloc_free_with_children/512                       12202 ns        12202 ns        56557
BM_h_halloc_alloc_free_with_children/4096                     103574 ns       103573 ns         6471
BM_h_halloc_alloc_free_with_children/8192                     216254 ns       216246 ns         3216
BM_h_stb_alloc_free_with_children/8                              181 ns          181 ns      3864757
BM_h_stb_alloc_free_with_children/64                            1401 ns         1401 ns       491585
BM_h_stb_alloc_free_with_children/512                          11242 ns        11242 ns        59957
BM_h_stb_alloc_free_with_children/4096                         98051 ns        98047 ns         7092
BM_h_stb_alloc_free_with_children/8192                        205333 ns       205326 ns         3409
BM_hashs_stbds_hash_bytes_32                                    1.63 ns         1.63 ns    418349970
BM_hashs_stbds_hash_bytes_accidental_32                         2.23 ns         2.23 ns    309089401
BM_hashs_stbds_hash_bytes_murmur_32                             2.75 ns         2.75 ns    252926500
BM_hashs_stbds_hash_bytes_64                                    2.58 ns         2.58 ns    270496151
BM_hashs_stbds_siphash_bytes_chunk/4                            4.86 ns         4.86 ns    136408431
BM_hashs_stbds_siphash_bytes_chunk/8                            5.73 ns         5.73 ns    114194444
BM_hashs_stbds_siphash_bytes_chunk/16                           7.99 ns         7.99 ns     84841059
BM_hashs_stbds_siphash_bytes_chunk/32                           12.0 ns         12.0 ns     56354520
BM_hashs_stbds_siphash_bytes_chunk/64                           20.1 ns         20.1 ns     35048980
BM_hashs_stbds_siphash_bytes_chunk/256                          74.4 ns         74.4 ns      9271830
BM_hashs_stbds_siphash_2_4_bytes_chunk/4                        10.6 ns         10.6 ns     64597923
BM_hashs_stbds_siphash_2_4_bytes_chunk/8                        13.1 ns         13.1 ns     53640350
BM_hashs_stbds_siphash_2_4_bytes_chunk/16                       17.0 ns         17.0 ns     40771580
BM_hashs_stbds_siphash_2_4_bytes_chunk/32                       24.5 ns         24.5 ns     28379591
BM_hashs_stbds_siphash_2_4_bytes_chunk/64                       39.9 ns         39.9 ns     17499328
BM_hashs_stbds_siphash_2_4_bytes_chunk/256                       134 ns          134 ns      5101968
BM_hashs_fasthash_32_chunk/4                                    5.15 ns         5.15 ns    128435025
BM_hashs_fasthash_32_chunk/8                                    3.75 ns         3.75 ns    187718529
BM_hashs_fasthash_32_chunk/16                                   4.85 ns         4.84 ns    144072815
BM_hashs_fasthash_32_chunk/32                                   7.17 ns         7.17 ns     93973575
BM_hashs_fasthash_32_chunk/64                                   11.8 ns         11.8 ns     58960543
BM_hashs_fasthash_32_chunk/256                                  44.3 ns         44.3 ns     15642634
BM_hashs_murmur2_chunk/4                                        4.28 ns         4.28 ns    160171034
BM_hashs_murmur2_chunk/8                                        5.19 ns         5.19 ns    124375199
BM_hashs_murmur2_chunk/16                                       6.85 ns         6.85 ns    100210680
BM_hashs_murmur2_chunk/32                                       10.3 ns         10.3 ns     66314158
BM_hashs_murmur2_chunk/64                                       17.3 ns         17.3 ns     40577752
BM_hashs_murmur2_chunk/256                                      71.0 ns         71.0 ns      9585745
BM_hashs_murmur3_chunk/4                                        4.90 ns         4.90 ns    142787770
BM_hashs_murmur3_chunk/8                                        5.98 ns         5.98 ns    112268509
BM_hashs_murmur3_chunk/16                                       8.35 ns         8.35 ns     83917088
BM_hashs_murmur3_chunk/32                                       12.5 ns         12.5 ns     52383541
BM_hashs_murmur3_chunk/64                                       23.1 ns         23.1 ns     29956608
BM_hashs_murmur3_chunk/256                                      90.0 ns         90.0 ns      7557953
BM_hashs_fasthash_64_chunk/4                                    5.16 ns         5.16 ns    130337420
BM_hashs_fasthash_64_chunk/8                                    3.71 ns         3.71 ns    187775039
BM_hashs_fasthash_64_chunk/16                                   4.85 ns         4.85 ns    135942793
BM_hashs_fasthash_64_chunk/32                                   7.23 ns         7.23 ns     94617846
BM_hashs_fasthash_64_chunk/64                                   11.8 ns         11.8 ns     58530990
BM_hashs_fasthash_64_chunk/256                                  44.0 ns         44.0 ns     15975924
BM_hashs_murmur2_64_chunk/4                                     4.59 ns         4.59 ns    153125236
BM_hashs_murmur2_64_chunk/8                                     4.29 ns         4.29 ns    163004207
BM_hashs_murmur2_64_chunk/16                                    5.12 ns         5.12 ns    132515967
BM_hashs_murmur2_64_chunk/32                                    6.87 ns         6.87 ns     99431900
BM_hashs_murmur2_64_chunk/64                                    10.3 ns         10.3 ns     65174332
BM_hashs_murmur2_64_chunk/256                                   36.5 ns         36.5 ns     18978417
BM_hashs_murmur3_64_chunk/4                                     8.71 ns         8.71 ns     79839874
BM_hashs_murmur3_64_chunk/8                                     9.74 ns         9.74 ns     70017598
BM_hashs_murmur3_64_chunk/16                                    8.03 ns         8.03 ns     84420534
BM_hashs_murmur3_64_chunk/32                                    9.96 ns         9.96 ns     68345297
BM_hashs_murmur3_64_chunk/64                                    14.3 ns         14.3 ns     47975639
BM_hashs_murmur3_64_chunk/256                                   41.6 ns         41.6 ns     16738627
```

## Setup

This project uses cmake and [vcpkg](https://vcpkg.io/en/) as dependencies manager.

Downloading vcpkg is not necessary since we use the json manifest in combination
with git submodules and [direnv](https://direnv.net/).

envrc is only needed to add vcpkg submodule to PATH and boostrap its binaries but both
can be done by hand with little extra effort.

### Steps

1. clone repo with submodules `git clone --recurse-submodules https://github.com/anvouk/anv_benchmarks.git`
2. If direnv is installed: `direnv allow .`
3. If direnv is NOT installed: `cd vcpkg && ./bootstrap-vcpkg.sh`

Gluing together vcpkg deps and the benchmarks should now be handled by cmake
without further steps.

## License

All tested libraries are under their respective licenses (see include headers).

Remaining code is under MIT License.
