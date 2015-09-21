cmake_minimum_required (VERSION 2.6)
project (crucible)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

# Added in Sentinel because CMake is fucking retarded
# TODO(athilenius): Fix the retarded dependency clashes in CMake
#add_subdirectory (/root/thilenius/utils/differencer
                  #/root/thilenius_bin/utils/differencer)
add_subdirectory (/root/thilenius/cloud/sentinel
                  /root/thilenius_bin/cloud/sentinel)

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

add_library (
  crucible
  /root/thilenius/scorch/cloud/crucible/crucible_client.cc
  /root/thilenius/scorch/cloud/crucible/crucible_mapper.cc
  /root/thilenius/scorch/cloud/crucible/crucible_model.cc
  /root/thilenius/scorch/cloud/crucible/crucible_repo.cc
  /root/thilenius_gen/scorch/cloud/crucible/Crucible.cpp
  /root/thilenius_gen/scorch/cloud/crucible/crucible_constants.cpp
  /root/thilenius_gen/scorch/cloud/crucible/crucible_types.cpp
)

target_link_libraries (
  crucible
  #differencer
  sentinel
)
