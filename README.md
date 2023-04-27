# benchmarks

Current benchmarks results:
```
Run on (36 X 3600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x18)
  L1 Instruction 32 KiB (x18)
  L2 Unified 256 KiB (x18)
  L3 Unified 46080 KiB (x1)
Load Average: 4.10, 2.20, 1.49
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------------------------------
Benchmark                                                       Time             CPU   Iterations
-------------------------------------------------------------------------------------------------
BM_arr_memmove                                               50.9 ns         50.9 ns     13684614
BM_arr_memcpy                                                50.5 ns         50.5 ns     13262457
BM_arr_memcpy_same_block                                     41.6 ns         41.6 ns     16259243
BM_arr_anv_arr_push_preallocated                             4.60 ns         4.60 ns    153559543
BM_arr_anv_arr_push                                          4.65 ns         4.60 ns    135580005
BM_arr_kvec_push_preallocated                                1.12 ns         1.12 ns    610344160
BM_arr_kvec_push                                            0.887 ns        0.886 ns    769070879
BM_arr_stb_ds_push_preallocated                              2.35 ns         2.35 ns    299471618
BM_arr_stb_ds_push                                           2.43 ns         2.43 ns    287514855
BM_h_malloc_new_free                                         10.7 ns         10.7 ns     65119888
BM_h_halloc_new_free_root                                    17.7 ns         17.7 ns     40017212
BM_h_anv_halloc_new_free_root                                19.9 ns         19.9 ns     35550932
BM_h_halloc_new_children                                     23.2 ns         23.2 ns     26312755
BM_h_anv_halloc_new_children/0                               20.5 ns         20.5 ns     34127356
BM_h_anv_halloc_new_children/8                               21.5 ns         21.5 ns     33262599
BM_h_anv_halloc_new_children/64                              63.0 ns         63.0 ns     11146291
BM_h_anv_halloc_new_children/512                              138 ns          138 ns      5123449
BM_h_anv_halloc_new_children/4096                             674 ns          674 ns      1025636
BM_h_anv_halloc_new_children/8192                            1906 ns         1906 ns       359289
BM_h_halloc_alloc_free_with_children/8                        198 ns          198 ns      3501940
BM_h_halloc_alloc_free_with_children/64                      1640 ns         1639 ns       423105
BM_h_halloc_alloc_free_with_children/512                    13142 ns        13130 ns        54255
BM_h_halloc_alloc_free_with_children/4096                  116179 ns       116174 ns         5881
BM_h_halloc_alloc_free_with_children/8192                  253313 ns       253287 ns         2804
BM_h_anv_halloc_alloc_free_with_children/8                    187 ns          187 ns      3733542
BM_h_anv_halloc_alloc_free_with_children/64                  1277 ns         1277 ns       547987
BM_h_anv_halloc_alloc_free_with_children/512                 9950 ns         9950 ns        70398
BM_h_anv_halloc_alloc_free_with_children/4096               79564 ns        79564 ns         8749
BM_h_anv_halloc_alloc_free_with_children/8192              159514 ns       159513 ns         4394
BM_h_anv_halloc_alloc_free_with_children_children/8           246 ns          246 ns      2890874
BM_h_anv_halloc_alloc_free_with_children_children/64         3338 ns         3338 ns       210550
BM_h_anv_halloc_alloc_free_with_children_children/512       62888 ns        62886 ns        10973
BM_h_anv_halloc_alloc_free_with_children_children/4096    2756178 ns      2756105 ns          254
BM_h_anv_halloc_alloc_free_with_children_children/8192   15619646 ns     15619583 ns           44
```
