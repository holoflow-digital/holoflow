add_executable(batched_spsc_queue_benchmarks benchmarks.cc)

set_common_target_properties(batched_spsc_queue_benchmarks)
set_common_compile_options(batched_spsc_queue_benchmarks)

target_link_libraries(batched_spsc_queue_benchmarks
    batched_spsc_queue
    benchmark::benchmark
)
