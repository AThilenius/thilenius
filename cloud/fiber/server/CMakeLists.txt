cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (fiber_server)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/cloud/fiber
                  /root/thilenius_bin/cloud/fiber)

add_executable (
  server
  /root/thilenius/cloud/fiber/server/server.cc
  /root/thilenius/cloud/fiber/server/fiber_handler.cc
)

target_link_libraries (
  server
  fiber
  gflags
)
