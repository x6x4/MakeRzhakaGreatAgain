cmake_minimum_required(VERSION 3.25)
project(Dummy)

add_subdirectory(ast)
add_subdirectory(runtime)

find_package(FLEX  REQUIRED)
find_package(BISON REQUIRED)

add_compile_options(-Wno-register -g)

FLEX_TARGET(
    BackendScanner
    dummy_scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
)

BISON_TARGET(
    BackendParser
    dummy_parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    COMPILE_FLAGS "-Wcounterexamples -Hparser.hpp"
)

ADD_FLEX_BISON_DEPENDENCY(BackendScanner BackendParser)

add_library(bisonflex STATIC
    ${BISON_BackendParser_OUTPUTS}
    ${FLEX_BackendScanner_OUTPUTS}
    scanner.h
)

target_include_directories(bisonflex SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(bisonflex PUBLIC        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bisonflex PUBLIC ast runtime)

add_executable(backend
    main.cpp
)

target_link_libraries(backend PUBLIC ast runtime bisonflex)