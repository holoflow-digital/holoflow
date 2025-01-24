add_executable(batched_spsc_queue_tests capacity_tests.cc multithread_tests.cc)

set_common_target_properties(batched_spsc_queue_tests)
set_common_compile_options(batched_spsc_queue_tests)

target_link_libraries(batched_spsc_queue_tests
    batched_spsc_queue
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(batched_spsc_queue_tests)
