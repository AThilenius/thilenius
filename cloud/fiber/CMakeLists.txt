cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (fiber)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

#add_subdirectory (/root/thilenius/base
                  #/root/thilenius_bin/base)
#add_subdirectory (/root/thilenius/third_party/mongo
                  #/root/thilenius_bin/third_party/mongo)
#add_subdirectory (/root/thilenius/third_party/thrift
                  #/root/thilenius_bin/third_party/thrift)
add_subdirectory (/root/thilenius/cloud/sentinel
                  /root/thilenius_bin/cloud/sentinel)

file(MAKE_DIRECTORY /root/thilenius_gen/cloud/fiber)
add_custom_command(
  OUTPUT
    /root/thilenius_gen/cloud/fiber/Fiber.cpp
    /root/thilenius_gen/cloud/fiber/fiber_constants.cpp
    /root/thilenius_gen/cloud/fiber/fiber_types.cpp
  COMMAND
    thrift -r --gen cpp -I /root/thilenius
    -out /root/thilenius_gen/cloud/fiber
    /root/thilenius/cloud/fiber/fiber.thrift
  DEPENDS
    thrift
    /root/thilenius/cloud/fiber/fiber.thrift
)

add_library (
  fiber
  /root/thilenius/cloud/fiber/fiber_client.cc
  /root/thilenius/cloud/fiber/fiber_mapper.cc
  /root/thilenius/cloud/fiber/fiber_model.cc
  /root/thilenius_gen/cloud/fiber/Fiber.cpp
  /root/thilenius_gen/cloud/fiber/fiber_constants.cpp
  /root/thilenius_gen/cloud/fiber/fiber_types.cpp
)

target_link_libraries (
  fiber
  #base
  #gflags
  #mongo
  sentinel
  #thrift
)
