
add_library(listener STATIC listener.cpp listener.hpp)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
target_include_directories(listener PUBLIC ${PARENT_DIR})
target_link_libraries(listener PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
