cmake_minimum_required (VERSION 2.6)
project (anvil)

include_directories (
  "/root/thilenius/libs/cpp"
)

add_subdirectory (
  /root/thilenius/libs/cpp/base /root/thilenius_bin/scorch/anvil/lib_cpp_base
)

add_executable (
  anvil
  /root/thilenius/scorch/anvil/anvil.cc
)

target_link_libraries (
  anvil
  base
)
