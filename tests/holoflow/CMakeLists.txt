add_executable(tensor_tests tensor/descriptor_tests.cc tensor/tensor_tests.cc)

set_common_target_properties(tensor_tests)
set_common_compile_options(tensor_tests)

target_link_libraries(tensor_tests
    holoflow
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tensor_tests)
