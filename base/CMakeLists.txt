cmake_minimum_required (VERSION 2.6)
project (base)

set (
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

add_subdirectory (/root/thilenius/base/gflags)
add_subdirectory (/root/thilenius/base/glog)
add_subdirectory (/root/thilenius/base/gtest)

add_library (
  base
  /root/thilenius/base/string.cc
)

target_link_libraries (
  base
  libgflags
  libglog
  libgtest
)
