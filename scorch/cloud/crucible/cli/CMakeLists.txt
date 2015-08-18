cmake_minimum_required (VERSION 2.6)
project (crucible_cli)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)
add_subdirectory (/root/thilenius/third_party/thrift
                  /root/thilenius_bin/third_party/thrift)

file(MAKE_DIRECTORY /root/thilenius_gen/scorch/cloud/crucible/cli)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/scorch/cloud/crucible/cli/Crucible.cpp
    /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_constants.cpp
    /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/scorch/cloud/crucible/cli
    /root/thilenius/scorch/cloud/crucible/crucible.thrift
  DEPENDS
    libthrift
    /root/thilenius/scorch/cloud/crucible/crucible.thrift
)

#===  CLI  =====================================================================
add_executable (
  crucible_cli
  /root/thilenius/scorch/cloud/crucible/cli/crucible_cli.cc
  /root/thilenius_gen/scorch/cloud/crucible/cli/Crucible.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_types.cpp
  /root/thilenius_gen/utils/differencer/differencer_constants.cpp
  /root/thilenius_gen/utils/differencer/differencer_types.cpp
)

target_link_libraries (
  crucible_cli
  base
  libthrift
)

#===  CREATE  ==================================================================
add_executable (
  crucible_cli_create
  /root/thilenius/scorch/cloud/crucible/cli/crucible_cli_create.cc
  /root/thilenius_gen/scorch/cloud/crucible/cli/Crucible.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_types.cpp
  /root/thilenius_gen/utils/differencer/differencer_constants.cpp
  /root/thilenius_gen/utils/differencer/differencer_types.cpp
)

target_link_libraries (
  crucible_cli_create
  base
  libthrift
)

#===  CLONE  =-=================================================================
add_executable (
  crucible_cli_clone
  /root/thilenius/scorch/cloud/crucible/cli/crucible_cli_clone.cc
  /root/thilenius_gen/scorch/cloud/crucible/cli/Crucible.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/cli/crucible_types.cpp
  /root/thilenius_gen/utils/differencer/differencer_constants.cpp
  /root/thilenius_gen/utils/differencer/differencer_types.cpp
)

target_link_libraries (
  crucible_cli_clone
  base
  libthrift
)
