cmake_minimum_required (VERSION 2.6)
project (testing_framework)

find_package(Threads)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen"
)

add_library (
  testing_framework STATIC
  /root/thilenius/external/testing_framework/console_colors.cc
  /root/thilenius/external/testing_framework/test.cc
  /root/thilenius/external/testing_framework/test_runner.cc
)

target_link_libraries (
  testing_framework
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable (
  testing_framework_test
  /root/thilenius/external/testing_framework/main.cc
)

target_link_libraries (
  testing_framework_test
  testing_framework
)
