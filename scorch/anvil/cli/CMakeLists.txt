cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (anvil_cli)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/scorch/anvil
                  /root/thilenius_bin/scorch/anvil)

add_executable (
  cli
  /root/thilenius/scorch/anvil/cli/cli.cc
)

target_link_libraries (
  cli
  anvil
  gflags
)
