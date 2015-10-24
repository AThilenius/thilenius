cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (billet)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

#add_subdirectory (/root/thilenius/base
                  #/root/thilenius_bin/base)
#add_subdirectory (/root/thilenius/cloud/sentinel
                  #/root/thilenius_bin/cloud/sentinel)
add_subdirectory (/root/thilenius/scorch/cloud/crucible
                  /root/thilenius_bin/scorch/cloud/crucible)
#add_subdirectory (/root/thilenius/cloud/fiber
                  #/root/thilenius_bin/cloud/fiber)

file(MAKE_DIRECTORY /root/thilenius_gen/scorch/cloud/billet)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/scorch/cloud/billet/Billet.cpp
    /root/thilenius_gen/scorch/cloud/billet/billet_constants.cpp
    /root/thilenius_gen/scorch/cloud/billet/billet_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/scorch/cloud/billet
    /root/thilenius/scorch/cloud/billet/billet.thrift
  DEPENDS
    thrift
    /root/thilenius/scorch/cloud/billet/billet.thrift
)

add_library (
  billet
  /root/thilenius/scorch/cloud/billet/docker_run_harness.cc
  /root/thilenius_gen/scorch/cloud/billet/Billet.cpp
  /root/thilenius_gen/scorch/cloud/billet/billet_constants.cpp
  /root/thilenius_gen/scorch/cloud/billet/billet_types.cpp
)

target_link_libraries (
  billet
  crucible
  #fiber
)
