# benchmarks

Current benchmarks results:
```
Run on (36 X 3600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x18)
  L1 Instruction 32 KiB (x18)
  L2 Unified 256 KiB (x18)
  L3 Unified 46080 KiB (x1)
Load Average: 1.56, 1.20, 1.56
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_arr_memmove                                  62.0 ns         62.0 ns     10494434
BM_arr_memcpy                                   57.0 ns         57.0 ns     11930806
BM_arr_memcpy_same_block                        40.8 ns         40.8 ns     17012524
BM_arr_anv_arr_push_preallocated                4.65 ns         4.65 ns    151640407
BM_arr_anv_arr_push                             5.29 ns         5.29 ns    121147962
BM_arr_kvec_push_preallocated                   1.08 ns         1.08 ns    649822820
BM_arr_kvec_push                               0.845 ns        0.845 ns    800489021
BM_arr_stb_ds_push_preallocated                 1.37 ns         1.37 ns    518726109
BM_arr_stb_ds_push                              2.31 ns         2.31 ns    299695939
BM_h_malloc_new_free                            10.7 ns         10.7 ns     64279456
BM_h_halloc_new_free_root                       15.8 ns         15.8 ns     44548429
BM_h_stb_new_free_root                          14.4 ns         14.4 ns     49154631
BM_h_halloc_new_children                        22.9 ns         22.9 ns     26620141
BM_h_stb_new_children                           24.3 ns         24.3 ns     46506825
BM_h_halloc_alloc_free_with_children/8           206 ns          206 ns      3357222
BM_h_halloc_alloc_free_with_children/64         1572 ns         1572 ns       454020
BM_h_halloc_alloc_free_with_children/512       12402 ns        12401 ns        56428
BM_h_halloc_alloc_free_with_children/4096     105621 ns       105620 ns         6433
BM_h_halloc_alloc_free_with_children/8192     218636 ns       218622 ns         3140
BM_h_stb_alloc_free_with_children/8              191 ns          191 ns      3657016
BM_h_stb_alloc_free_with_children/64            1449 ns         1449 ns       476657
BM_h_stb_alloc_free_with_children/512          11635 ns        11635 ns        58582
BM_h_stb_alloc_free_with_children/4096        100125 ns       100119 ns         6927
BM_h_stb_alloc_free_with_children/8192        206641 ns       206640 ns         3333
```
