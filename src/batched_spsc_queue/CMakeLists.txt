add_library(batched_spsc_queue STATIC batched_spsc_queue.cc)

set_common_target_properties(batched_spsc_queue)
set_common_compile_options(batched_spsc_queue)

target_include_directories(batched_spsc_queue PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
