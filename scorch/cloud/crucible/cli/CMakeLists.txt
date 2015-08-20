cmake_minimum_required (VERSION 2.6)
project (crucible_cli)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/scorch/cloud/crucible
                  /root/thilenius_bin/scorch/cloud/crucible)

add_executable (
  cli
  /root/thilenius/scorch/cloud/crucible/cli/cli.cc
  /root/thilenius/scorch/cloud/crucible/cli/create.cc
  /root/thilenius/scorch/cloud/crucible/cli/status.cc
)

target_link_libraries (
  cli
  crucible
)
