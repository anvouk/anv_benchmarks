cmake_minimum_required(VERSION 3.24)

# Enable vcpkg.json versions
set(VCPKG_FEATURE_FLAGS "versions")

add_compile_options(-fpermissive)

# Tell cmake to use vcpkg submodule
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(benchmarks)

include_directories(include/arrays)
include_directories(include/hallocs)
include_directories(include/hashs)

add_library(MurmurHash
    include/hashs/MurmurHash2.h include/hashs/MurmurHash2.cpp
    include/hashs/MurmurHash3.h include/hashs/MurmurHash3.cpp
)

add_subdirectory(src)
