cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (anvil)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)
add_subdirectory (/root/thilenius/third_party/thrift
                  /root/thilenius_bin/third_party/thrift)

file(MAKE_DIRECTORY /root/thilenius_gen/scorch/anvil)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/scorch/anvil/Blaze.cpp
    /root/thilenius_gen/scorch/anvil/BlazeCommandProcessor.cpp
    /root/thilenius_gen/scorch/anvil/blaze_constants.cpp
    /root/thilenius_gen/scorch/anvil/blaze_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius -out /root/thilenius_gen/scorch/anvil
    /root/thilenius/scorch/cloud/blaze/blaze.thrift
  DEPENDS
    libthrift
    /root/thilenius/scorch/cloud/blaze/blaze.thrift
)

add_library (
  anvil STATIC
  /root/thilenius/scorch/anvil/anvil_client.cc
  /root/thilenius_gen/scorch/anvil/Blaze.cpp
  /root/thilenius_gen/scorch/anvil/BlazeCommandProcessor.cpp
  /root/thilenius_gen/scorch/anvil/blaze_constants.cpp
  /root/thilenius_gen/scorch/anvil/blaze_types.cpp
)

target_link_libraries (
  anvil
  base
  libthrift
)
