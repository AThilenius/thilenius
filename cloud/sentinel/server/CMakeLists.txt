cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (sentinel_server)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/cloud/sentinel
                  /root/thilenius_bin/cloud/sentinel)

add_executable (
  server
  /root/thilenius/cloud/sentinel/server/server.cc
  /root/thilenius/cloud/sentinel/server/sentinel_handler.cc
)

target_link_libraries (
  server
  sentinel
)
