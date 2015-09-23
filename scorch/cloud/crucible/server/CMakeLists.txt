cmake_minimum_required (VERSION 2.6)
project (crucible_server)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/scorch/cloud/crucible
                  /root/thilenius_bin/scorch/cloud/crucible)

add_executable (
  server
  /root/thilenius/scorch/cloud/crucible/server/server.cc
  /root/thilenius/scorch/cloud/crucible/server/crucible_handler.cc
)

target_link_libraries (
  server
  crucible
  /usr/local/lib/libthrift.a
  /usr/local/lib/libthriftnb.a
)
