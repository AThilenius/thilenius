cmake_minimum_required (VERSION 2.6)
project (base)

set (
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

add_subdirectory (
  /root/thilenius/base/gflags /root/thilenius_bin/scorch/anvil/base/gflags
)

add_library (
  base
  /root/thilenius/base/string.cc
)

target_link_libraries (
  base
  gflags
)
