cmake_minimum_required(VERSION 3.15)
project(ExamplePlugin)

# Build as shared library
add_library(example_plugin SHARED
    example_plugin.cpp
)

# Include directories
target_include_directories(example_plugin PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Set output directory
set_target_properties(example_plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

# Install
install(TARGETS example_plugin
    LIBRARY DESTINATION lib/customos/plugins
    RUNTIME DESTINATION bin/customos/plugins
)
