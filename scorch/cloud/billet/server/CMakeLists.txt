cmake_minimum_required (VERSION 2.6)
project (billet_server)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/scorch/cloud/billet
                  /root/thilenius_bin/scorch/cloud/billet)

add_executable (
  server
  /root/thilenius/scorch/cloud/billet/server/server.cc
  /root/thilenius/scorch/cloud/billet/server/billet_handler.cc
)

target_link_libraries (
  server
  billet
)
