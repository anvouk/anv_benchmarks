# anv_benchmarks

benchmarks about single header C libraries.

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
Load Average: 0.88, 1.12, 1.21
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------------------
Benchmark                                                          Time             CPU   Iterations
----------------------------------------------------------------------------------------------------
BM_libc_memmove/8                                               2.60 ns         2.60 ns    271141963
BM_libc_memmove/64                                              2.31 ns         2.31 ns    303950037
BM_libc_memmove/4096                                            54.7 ns         54.7 ns     12439589
BM_libc_memcpy/8                                                2.33 ns         2.33 ns    299514547
BM_libc_memcpy/64                                               2.01 ns         2.01 ns    348194289
BM_libc_memcpy/4096                                             54.4 ns         54.3 ns     12508066
BM_libc_malloc_and_free                                         10.7 ns         10.7 ns     64851985
BM_arr_anv_arr_push                                             4.63 ns         4.63 ns    152459502
BM_arr_kvec_push                                               0.871 ns        0.870 ns    779495483
BM_arr_stb_ds_push                                              2.37 ns         2.37 ns    295083925
BM_arr_stb_stretchy_push/iterations:50000000                    2.35 ns         2.35 ns     50000000
BM_arr_anv_arr_push_preallocated                                5.16 ns         5.16 ns    134142008
BM_arr_kvec_push_preallocated                                   1.11 ns         1.11 ns    632073603
BM_arr_stb_ds_push_preallocated                                 1.39 ns         1.39 ns    497395686
BM_arr_stb_stretchy_push_preallocated/iterations:50000000       2.32 ns         2.32 ns     50000000
BM_h_halloc_new_free_root                                       15.7 ns         15.7 ns     44357546
BM_h_stb_new_free_root                                          14.2 ns         14.2 ns     50146489
BM_h_halloc_new_children                                        23.4 ns         23.4 ns     26600326
BM_h_stb_new_children                                           24.7 ns         24.7 ns     47108630
BM_h_halloc_alloc_free_with_children/8                           214 ns          214 ns      3323670
BM_h_halloc_alloc_free_with_children/64                         1558 ns         1558 ns       449666
BM_h_halloc_alloc_free_with_children/512                       12304 ns        12303 ns        55954
BM_h_halloc_alloc_free_with_children/4096                     103602 ns       103602 ns         6696
BM_h_halloc_alloc_free_with_children/8192                     218104 ns       218101 ns         3198
BM_h_stb_alloc_free_with_children/8                              192 ns          192 ns      3620253
BM_h_stb_alloc_free_with_children/64                            1387 ns         1387 ns       499025
BM_h_stb_alloc_free_with_children/512                          11155 ns        11154 ns        62565
BM_h_stb_alloc_free_with_children/4096                         98612 ns        98606 ns         6997
BM_h_stb_alloc_free_with_children/8192                        203083 ns       203079 ns         3358
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
