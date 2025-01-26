add_executable(tensor_example tensor/example.cc)

set_common_target_properties(tensor_example)
set_common_compile_options(tensor_example)

target_link_libraries(tensor_example
    holoflow
)
