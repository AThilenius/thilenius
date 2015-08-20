cmake_minimum_required (VERSION 2.6)
project (crucible_server)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(
  Boost 1.58.0 COMPONENTS
  regex
  system
  thread
)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/third_party/mongo
                  /root/thilenius_bin/third_party/mongo)
add_subdirectory (/root/thilenius/scorch/cloud/crucible
                  /root/thilenius_bin/scorch/cloud/crucible)

add_executable (
  server
  /root/thilenius/scorch/cloud/crucible/server/server.cc
  /root/thilenius/scorch/cloud/crucible/server/crucible_handler.cc
)

target_link_libraries (
  server
  ${Boost_LIBRARIES}
  crucible
  libmongoclient
)
