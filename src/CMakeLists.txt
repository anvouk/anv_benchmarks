set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=haswell -mavx)

add_executable(benchmarks main.cpp libc.cpp arrays.cpp hallocs.cpp utils.h hashs.cpp)

target_link_libraries(benchmarks PRIVATE MurmurHash)

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark benchmark::benchmark_main)
