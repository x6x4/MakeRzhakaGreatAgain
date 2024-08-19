
add_library(ast STATIC ast.cpp ast.h)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
target_include_directories(ast PUBLIC ${PARENT_DIR})