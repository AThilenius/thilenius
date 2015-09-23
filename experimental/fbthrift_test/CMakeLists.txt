cmake_minimum_required (VERSION 2.6)
project (fbthrift_test)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (
  "/root/thilenius/"
  "/root/thilenius_gen/"
)

add_executable (
  fbthrift_test
  #/root/thilenius/experimental/fbthrift_test/server.cpp
  /root/thilenius/experimental/fbthrift_test/old_server.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/Calculator.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/calculator_constants.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/calculator_types.cpp
  /root/thilenius/experimental/fbthrift_test/gen-cpp/Calculator.cpp
  /root/thilenius/experimental/fbthrift_test/gen-cpp/calculator_constants.cpp
  /root/thilenius/experimental/fbthrift_test/gen-cpp/calculator_types.cpp
)

target_link_libraries (
  fbthrift_test
  #boost_system
  pthread
  #glog
  #folly
  #thrift
  #thriftcpp2
  /usr/local/lib/libthrift.a
)

#add_executable (
  #client
  #/root/thilenius/experimental/fbthrift_test/client.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/Calculator.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/Calculator_client.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/calculator_constants.cpp
  #/root/thilenius/experimental/fbthrift_test/gen-cpp2/calculator_types.cpp
#)

#target_link_libraries (
  #client
  #boost_system
  #pthread
  #glog
  #folly
  #thrift
  #thriftcpp2
#)
