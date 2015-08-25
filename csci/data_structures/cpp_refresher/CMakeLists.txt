cmake_minimum_required (VERSION 2.6)
project (cpp_refresher)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/scorch/testing_framework
                  /root/thilenius_bin/scorch/testing_framework)

add_executable (
  cpp_refresher
  /root/thilenius/csci/data_structures/cpp_refresher/cpp_refresher.cc
  /root/thilenius/csci/data_structures/cpp_refresher/unit_tests.cc
)

target_link_libraries (
  cpp_refresher
  testing_framework
)
