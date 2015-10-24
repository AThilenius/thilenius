cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (differencer)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)
add_subdirectory (/root/thilenius/third_party/thrift
                  /root/thilenius_bin/third_party/thrift)

file(MAKE_DIRECTORY /root/thilenius_gen/utils/differencer)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/utils/differencer/differencer_constants.cpp
    /root/thilenius_gen/utils/differencer/differencer_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/utils/differencer
    /root/thilenius/utils/differencer/differencer.thrift
  DEPENDS
    thrift
    /root/thilenius/utils/differencer/differencer.thrift
)

add_library (
  differencer
  /root/thilenius/utils/differencer/differencer.cc
  /root/thilenius/utils/differencer/differencer_mapper.cc
  /root/thilenius_gen/utils/differencer/differencer_constants.cpp
  /root/thilenius_gen/utils/differencer/differencer_types.cpp
)

target_link_libraries (
  differencer
  base
  thrift
)

#===  TESTS ====================================================================

add_executable (
  differencer_test
  /root/thilenius/utils/differencer/differencer_test.cc
)

target_link_libraries (
  differencer_test
  base
  differencer
  libgtest_main
  thrift
)
