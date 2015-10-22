cmake_minimum_required (VERSION 2.6)
project (fiber_cli)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/cloud/fiber
                  /root/thilenius_bin/cloud/fiber)

add_executable (
  cli
  /root/thilenius/cloud/fiber/cli/cli.cc
)

target_link_libraries (
  cli
  fiber
)
