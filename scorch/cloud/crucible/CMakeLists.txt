cmake_minimum_required (VERSION 2.6)
project (crucible)

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

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)
add_subdirectory (/root/thilenius/third_party/mongo
                  /root/thilenius_bin/third_party/mongo)
add_subdirectory (/root/thilenius/third_party/thrift
                  /root/thilenius_bin/third_party/thrift)

file(MAKE_DIRECTORY /root/thilenius_gen/scorch/cloud/crucible)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/scorch/cloud/crucible/Crucible.cpp
    /root/thilenius_gen/scorch/cloud/crucible/crucible_constants.cpp
    /root/thilenius_gen/scorch/cloud/crucible/crucible_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/scorch/cloud/crucible
    /root/thilenius/scorch/cloud/crucible/crucible.thrift
  DEPENDS
    libthrift
    /root/thilenius/scorch/cloud/crucible/crucible.thrift
    /root/thilenius/utils/differencer/differencer.thrift
)

add_executable (
  crucible
  /root/thilenius/scorch/cloud/crucible/crucible.cc
  /root/thilenius/scorch/cloud/crucible/crucible_handler.cc
  /root/thilenius/scorch/cloud/crucible/crucible_model.cc
  /root/thilenius_gen/scorch/cloud/crucible/Crucible.cpp
  /root/thilenius_gen/scorch/cloud/crucible/crucible_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/crucible_types.cpp
  /root/thilenius_gen/scorch/cloud/crucible/differencer_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/differencer_types.cpp
)

target_link_libraries (
  crucible
  ${Boost_LIBRARIES}
  base
  libmongoclient
  libthrift
)
