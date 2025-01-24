add_executable(batched_spsc_queue_example example.cc)

set_common_target_properties(batched_spsc_queue_example)
set_common_compile_options(batched_spsc_queue_example)

target_link_libraries(batched_spsc_queue_example
    batched_spsc_queue
    Threads::Threads
)
