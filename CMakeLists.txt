cmake_minimum_required(VERSION 3.18)
project(holoflow)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CommonCMakeFunctions)

# Options
option(USE_VCPKG "Use vcpkg for dependencies" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_DOCUMENTATIONS "Build documentations" ON)

# Dependencies
find_package(GTest CONFIG REQUIRED)
find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)
find_package(Doxygen)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(docs)
add_subdirectory(examples)
