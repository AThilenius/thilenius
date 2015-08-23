cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install zlib
set(zlib_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/zlib)
ExternalProject_Add(
    zlib
    PREFIX ${zlib_PREFIX}
    URL http://zlib.net/zlib128.zip
    URL_MD5 126f8676442ffbd97884eb4d6f32afb4
)
include_directories(${zlib_PREFIX}/include)
set(zlib_LIBRARIES /usr/local/lib/libz.a)

# Create a libzlib target to be used as a dependency by test programs
add_library(libz IMPORTED STATIC GLOBAL)
add_dependencies(libz zlib)

# Set zlib properties
ExternalProject_Get_Property(zlib source_dir binary_dir)
set_target_properties(libz PROPERTIES
    "IMPORTED_LOCATION" "${zlib_LIBRARIES}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
