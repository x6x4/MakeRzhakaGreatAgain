add_compile_options(-Wno-register -g)
add_library(runtime STATIC runtime.cpp runtime.h)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
target_include_directories(ast PUBLIC ${PARENT_DIR})