cmake_minimum_required (VERSION 2.6)
project (base)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We need thread support for gtools
find_package(Threads REQUIRED)
include(ExternalProject)

#===  BOOST  ===================================================================
ExternalProject_Add(
  boost
  URL http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz
  BUILD_IN_SOURCE 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ./bootstrap.sh
  BUILD_COMMAND  ./b2 install
    --without-python
    --without-mpi
    --disable-icu
    --prefix=/usr
    --threading=multi
    --variant=release
    -s NO_BZIP2=1
    -q
    -j8
  # Handled by the build command
  INSTALL_COMMAND ""
  INSTALL_DIR /usr
)

ExternalProject_Add(
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

#===  GOOGLE FLAGS  ============================================================
ExternalProject_Add(
    gflags
    URL https://github.com/gflags/gflags/archive/v2.1.2.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gflags
)

# Create a libgflags target
add_library(libgflags IMPORTED STATIC GLOBAL)
add_dependencies(libgflags gflags)

# Set gflags properties
ExternalProject_Get_Property(gflags source_dir binary_dir)
set_target_properties(libgflags PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgflags.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

#===  GOOGLE TEST  =============================================================
ExternalProject_Add(
    gtest
    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    DEPENDS gflags
    # Disable install step
    INSTALL_COMMAND ""
)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libgtest_main IMPORTED STATIC GLOBAL)
add_dependencies(libgtest_main gtest)

set_target_properties(libgtest_main PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest_main.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

#===  GOOGLE MOCK  =============================================================
ExternalProject_Add(
    gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
    DEPENDS gflags
    INSTALL_COMMAND ""
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)

# Set gmock properties
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

#===  ZLIB  ====================================================================
set(zlib_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/zlib)
ExternalProject_Add(
    zlib
    PREFIX ${zlib_PREFIX}
    URL http://zlib.net/zlib128.zip
    URL_MD5 126f8676442ffbd97884eb4d6f32afb4
)

# Create a libzlib target to be used as a dependency by test programs
add_library(libz IMPORTED STATIC GLOBAL)
add_dependencies(libz zlib)

# Set zlib properties
ExternalProject_Get_Property(zlib source_dir binary_dir)
set_target_properties(libz PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libz.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")


#===  CURL  ====================================================================
set(curl_prefix ${CMAKE_CURRENT_BINARY_DIR}/curl)
ExternalProject_Add(
  curl
  PREFIX ${curl_prefix}
  URL http://curl.haxx.se/download/curl-7.44.0.tar.gz
  CONFIGURE_COMMAND ./configure --disable-shared
  BUILD_IN_SOURCE 1
)

# Create a libthrift target to be used as a dependency by test programs
add_library(libcurl IMPORTED STATIC GLOBAL)
add_dependencies(
  libcurl
  curl
  libz
  ssl
)

# Set thrift properties
ExternalProject_Get_Property(curl source_dir binary_dir)
set_target_properties(libcurl PROPERTIES
    "IMPORTED_LOCATION" "/usr/local/lib/libcurl.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#===  BASE LIB  ================================================================
include_directories("/root/thilenius")
#set(Boost_USE_STATIC_LIBS ON)
#find_package(Boost COMPONENTS regex system filesystem REQUIRED)

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
  boost
  boost_process
  libcurl
  libgflags
  libgmock
  libgtest
  libz
)

target_link_libraries (
  base
  boost_regex
  boost_filesystem
  boost_system
  crypto
  libcurl
  libgflags
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
