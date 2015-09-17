cmake_minimum_required (VERSION 2.6)
project (sentinel)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

#add_subdirectory (/root/thilenius/base
                  #/root/thilenius_bin/base)
#add_subdirectory (/root/thilenius/third_party/thrift
                  #/root/thilenius_bin/third_party/thrift)
add_subdirectory (/root/thilenius/third_party/mongo
                  /root/thilenius_bin/third_party/mongo)
# This needs to be here for now to resolve the fact that the differ, sentinal
# and crucible all need thrift
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
    libthrift
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
  #libthrift
  differencer
  libmongoclient
)
