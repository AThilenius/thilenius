cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (sentinel)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

#add_subdirectory (/root/thilenius/base
                  #/root/thilenius_bin/base)
add_subdirectory (/root/thilenius/third_party/mongo
                  /root/thilenius_bin/third_party/mongo)
#add_subdirectory (/root/thilenius/third_party/thrift
                  #/root/thilenius_bin/third_party/thrift)

# Must be here because CMake if a fucking hunk of shit
add_subdirectory (/root/thilenius/utils/differencer
                  /root/thilenius_bin/utils/differencer)


file(MAKE_DIRECTORY /root/thilenius_gen/cloud/sentinel)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/cloud/sentinel/Sentinel.cpp
    /root/thilenius_gen/cloud/sentinel/SentinelInternal.cpp
    /root/thilenius_gen/cloud/sentinel/sentinel_constants.cpp
    /root/thilenius_gen/cloud/sentinel/sentinel_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/cloud/sentinel
    /root/thilenius/cloud/sentinel/sentinel.thrift
  DEPENDS
    thrift
    /root/thilenius/cloud/sentinel/sentinel.thrift
)

add_library (
  sentinel
  /root/thilenius/cloud/sentinel/sentinel_client.cc
  /root/thilenius/cloud/sentinel/sentinel_mapper.cc
  /root/thilenius/cloud/sentinel/sentinel_model.cc
  /root/thilenius_gen/cloud/sentinel/Sentinel.cpp
  /root/thilenius_gen/cloud/sentinel/SentinelInternal.cpp
  /root/thilenius_gen/cloud/sentinel/sentinel_constants.cpp
  /root/thilenius_gen/cloud/sentinel/sentinel_types.cpp
)

target_link_libraries (
  sentinel
  #base
  differencer
  gflags
  mongo
  thrift
)
