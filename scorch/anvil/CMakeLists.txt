cmake_minimum_required (VERSION 2.6)
project (anvil)

set (
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

include_directories (
  "/root/thilenius"
)

add_subdirectory (
  /root/thilenius/base /root/thilenius_bin/scorch/anvil/base
)

add_executable (
  anvil
  /root/thilenius/scorch/anvil/anvil.cc
)

target_link_libraries (
  anvil
  base
)
