# This functions is used to set common target properties. This is used for all
# targets in the project.
function(set_common_target_properties target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
endfunction()

# This function is used to set common compile options. This is used for all
# targets in the project.
function(set_common_compile_options target)
    target_compile_options(${target} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wall
        -Wextra
        -Wpedantic
        -Wno-array-bounds
        -Wno-stringop-overflow
    >
)
endfunction()
