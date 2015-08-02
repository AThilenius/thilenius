cmake_minimum_required (VERSION 2.6)
project (anvil)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")

set (
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

add_subdirectory (/root/thilenius/base /root/thilenius_bin/scorch/anvil/base)
add_subdirectory (/root/thilenius/third_party/thrift
                  /root/thilenius_bin/scorch/anvil/third_party/thrift)

add_executable (
  anvil
  /root/thilenius/scorch/anvil/anvil.cc
)

target_link_libraries (
  anvil
  base
  libthrift
)
