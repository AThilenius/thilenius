cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (base)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We need thread support for gtools
find_package(Threads REQUIRED)
include(ExternalProject)

ExternalProject_Add (
  boost_process
  URL http://www.highscore.de/boost/process.zip
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND cp -rv
    ${CMAKE_CURRENT_BINARY_DIR}/boost_process-prefix/src/boost_process/boost
    /usr/include
    COMMAND sed -i -e
      "s*<boost/filesystem/path.hpp>*<boost/filesystem/operations.hpp>*g"
      /usr/include/boost/process/operations.hpp
)

#===  GOOGLE TEST  =============================================================
ExternalProject_Add (
  gtest
  URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
  INSTALL_COMMAND ""
)
# Gtest
ExternalProject_Get_Property(gtest source_dir binary_dir)
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#Gtest main
add_library(libgtest_main IMPORTED STATIC GLOBAL)
add_dependencies(libgtest_main gtest)
set_target_properties(libgtest_main PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest_main.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#===  GOOGLE MOCK  =============================================================
ExternalProject_Add (
  gmock
  URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
  INSTALL_COMMAND ""
)
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)

ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#===  ZLIB  ====================================================================
ExternalProject_Add (
  zlib
  URL http://zlib.net/zlib128.zip
)
add_library(libz IMPORTED STATIC GLOBAL)
add_dependencies(libz zlib)

set_target_properties(libz PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libz.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#===  CURL  ====================================================================
ExternalProject_Add (
  curl
  URL http://curl.haxx.se/download/curl-7.44.0.tar.gz
  CONFIGURE_COMMAND ./configure --disable-shared
  BUILD_IN_SOURCE 1
)
add_library(libcurl IMPORTED STATIC GLOBAL)
add_dependencies(
  libcurl
  curl
  libz
  ssl
)

set_target_properties(libcurl PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libcurl.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#===  BASE LIB  ================================================================
include_directories("/root/thilenius")

add_library (
  base STATIC
  /root/thilenius/base/arguments.cc
  /root/thilenius/base/auto_update.cc
  /root/thilenius/base/crypto.cc
  /root/thilenius/base/dependency.cc
  /root/thilenius/base/directory.cc
  /root/thilenius/base/file.cc
  /root/thilenius/base/guid.cc
  /root/thilenius/base/http.cc
  /root/thilenius/base/log.cc
  /root/thilenius/base/path.cc
  /root/thilenius/base/process.cc
  /root/thilenius/base/regex.cc
  /root/thilenius/base/string.cc
  /root/thilenius/base/text_file.cc
  /root/thilenius/base/time.cc
)

add_dependencies (
  base
  boost_process
  libcurl
  libgmock
  libgtest
  libz
)

target_link_libraries (
  base
  boost_filesystem
  boost_regex
  boost_system
  boost_thread
  crypto
  libcurl
  libgmock
  libgtest
  ssl
  libz
)

#===  BASE TESTS  ==============================================================
add_executable (
  base_test
  /root/thilenius/base/crypto_test.cc
  /root/thilenius/base/dependency_test.cc
  /root/thilenius/base/directory_test.cc
  /root/thilenius/base/file_test.cc
  /root/thilenius/base/http_test.cc
  /root/thilenius/base/path_test.cc
  /root/thilenius/base/process_test.cc
  /root/thilenius/base/regex_test.cc
  /root/thilenius/base/string_test.cc
  /root/thilenius/base/value_of_test.cc
)

target_link_libraries (
  base_test
  base
  libgtest_main
)
