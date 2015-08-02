cmake_minimum_required (VERSION 2.6)
project (sample_assignment)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
)

add_subdirectory (/root/thilenius/scorch/anvil /root/thilenius_bin/scorch/anvil)

add_executable (
  sample_assignment
  /root/thilenius/experimental/sample_assignment/main.cc
  /root/thilenius/experimental/sample_assignment/linked_list.cc
)

target_link_libraries (
  sample_assignment
  anvil
)
