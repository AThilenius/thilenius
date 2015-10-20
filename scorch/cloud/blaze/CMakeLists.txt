cmake_minimum_required (VERSION 2.6)
project (blaze)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/cloud/sentinel
                  /root/thilenius_bin/cloud/sentinel)

file(MAKE_DIRECTORY /root/thilenius_gen/scorch/cloud/blaze)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/scorch/cloud/blaze/Blaze.cpp
    /root/thilenius_gen/scorch/cloud/blaze/BlazeCommandProcessor.cpp
    /root/thilenius_gen/scorch/cloud/blaze/blaze_constants.cpp
    /root/thilenius_gen/scorch/cloud/blaze/blaze_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/scorch/cloud/blaze
    /root/thilenius/scorch/cloud/blaze/blaze.thrift
  DEPENDS
    libthrift
    /root/thilenius/scorch/cloud/blaze/blaze.thrift
)

add_executable (
  blaze
  /root/thilenius/scorch/cloud/blaze/blaze_handler.cc
  /root/thilenius/scorch/cloud/blaze/blaze_mapper.cc
  /root/thilenius/scorch/cloud/blaze/blaze_model.cc
  /root/thilenius/scorch/cloud/blaze/server.cc
  /root/thilenius_gen/scorch/cloud/blaze/Blaze.cpp
  /root/thilenius_gen/scorch/cloud/blaze/BlazeCommandProcessor.cpp
  /root/thilenius_gen/scorch/cloud/blaze/blaze_constants.cpp
  /root/thilenius_gen/scorch/cloud/blaze/blaze_types.cpp
)

target_link_libraries (
  blaze
  sentinel
)
