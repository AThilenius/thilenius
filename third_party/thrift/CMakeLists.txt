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

add_library(libthrift IMPORTED STATIC GLOBAL)
add_dependencies(libthrift thrift)
set_target_properties(libthrift PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libthrift.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libthriftnb IMPORTED STATIC GLOBAL)
add_dependencies(libthriftnb thrift)
set_target_properties(libthriftnb PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libthriftnb.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
