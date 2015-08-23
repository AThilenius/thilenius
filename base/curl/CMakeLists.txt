cmake_minimum_required (VERSION 2.6)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(/root/thilenius/base/zlib
                 /root/thilenius_bin/base/zlib)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install Thrift
set(curl_prefix ${CMAKE_CURRENT_BINARY_DIR}/curl)
ExternalProject_Add(
  curl
  PREFIX ${curl_prefix}
  URL http://curl.haxx.se/download/curl-7.44.0.tar.gz
  CONFIGURE_COMMAND ./configure --disable-shared
  BUILD_IN_SOURCE 1
)

# Create a libthrift target to be used as a dependency by test programs
add_library(libcurl IMPORTED STATIC GLOBAL)
add_dependencies(
  libcurl
  curl
  zlib
  ssl
)

# Set thrift properties
ExternalProject_Get_Property(curl source_dir binary_dir)
set_target_properties(libcurl PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libcurl.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
