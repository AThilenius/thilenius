cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

add_custom_command (COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/install_fbthrift)
