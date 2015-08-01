cmake_minimum_required (VERSION 2.6)
project (thrift)

set (
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

add_library (
  thrift
  empty.cc
)

target_link_libraries (
  thrift
  libthrift.a
)
