cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install Thrift
ExternalProject_Add(
  thrift
  URL http://www.trieuvan.com/apache/thrift/0.9.2/thrift-0.9.2.tar.gz
  CONFIGURE_COMMAND ./configure
    --enable-shared=no
    --without-tests
    --without-php
    --without-php_extension
    --without-ruby
    --without-haskell
    --without-d
    --without-go
    --without-perl
    --without-erlang
    --without-c_glib
  BUILD_IN_SOURCE 1
)
include_directories(${thrift_PREFIX}/include)
set(THRIFT_LIBRARIES /usr/local/lib/libthrift.a)

# Create a libthrift target to be used as a dependency by test programs
add_library(libthrift IMPORTED STATIC GLOBAL)
add_dependencies(libthrift thrift)

# Set thrift properties
ExternalProject_Get_Property(thrift source_dir binary_dir)
set_target_properties(libthrift PROPERTIES
    "IMPORTED_LOCATION" "${THRIFT_LIBRARIES}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
