cmake_minimum_required (VERSION 2.6)
project (test_code)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)

add_executable (
  test_code
  /root/thilenius/experimental/test_code/test_code.cc
)

target_link_libraries (
  test_code
  base
)
