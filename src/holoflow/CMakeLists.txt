add_library(holoflow STATIC tensor/descriptor.cc tensor/tensor.cc)

set_common_target_properties(holoflow)
set_common_compile_options(holoflow)

target_include_directories(holoflow PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(holoflow PUBLIC
    glog::glog
)
