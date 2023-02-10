cmake_minimum_required(VERSION 3.24)

# Enable vcpkg.json versions
set(VCPKG_FEATURE_FLAGS "versions")

# Tell cmake to use vcpkg submodule
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(benchmarks)

include_directories(include)

add_subdirectory(src)
