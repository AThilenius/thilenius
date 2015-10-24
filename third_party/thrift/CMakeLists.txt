cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (thrift)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install Thrift
ExternalProject_Add(
  thrift_proj
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

add_library(thrift IMPORTED STATIC GLOBAL)
add_dependencies(thrift thrift_proj)
set_target_properties(thrift PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libthrift.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(thriftnb IMPORTED STATIC GLOBAL)
add_dependencies(thriftnb thrift_proj)
set_target_properties(thriftnb PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libthriftnb.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
