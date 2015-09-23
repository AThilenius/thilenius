cmake_minimum_required (VERSION 2.6)
project (differencer)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/base)

file(MAKE_DIRECTORY /root/thilenius_gen/utils/differencer)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/utils/differencer/gen-cpp2/differencer_constants.cpp
    /root/thilenius_gen/utils/differencer/gen-cpp2/differencer_types.cpp
  COMMAND
    python -m thrift_compiler.main -r --gen cpp2 -I /root/thilenius
    --install /root/thilenius_gen/utils/differencer
    /root/thilenius/utils/differencer/differencer.thrift
  DEPENDS
    /root/thilenius/utils/differencer/differencer.thrift
)

add_library (
  differencer STATIC
  /root/thilenius/utils/differencer/differencer.cc
  /root/thilenius/utils/differencer/differencer_mapper.cc
  /root/thilenius_gen/utils/differencer/gen-cpp2/differencer_constants.cpp
  /root/thilenius_gen/utils/differencer/gen-cpp2/differencer_types.cpp
)

target_link_libraries (
  differencer
  base
  thriftcpp2
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
  thriftcpp2
)
