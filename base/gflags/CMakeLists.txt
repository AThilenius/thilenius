cmake_minimum_required (VERSION 2.6)

# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleFlags
ExternalProject_Add(
    gflags
    URL https://github.com/gflags/gflags/archive/v2.1.2.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gflags
)

# Create a libgflags target
add_library(libgflags IMPORTED STATIC GLOBAL)
add_dependencies(libgflags gflags)

# Set gflags properties
ExternalProject_Get_Property(gflags source_dir binary_dir)
set_target_properties(libgflags PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgflags.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
