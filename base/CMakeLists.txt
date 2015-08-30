cmake_minimum_required (VERSION 2.6)
project (base)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We need thread support for gtools
find_package(Threads REQUIRED)
include(ExternalProject)

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

#===  CURL, ZLIB  ==============================================================
add_subdirectory (/root/thilenius/base/curl)

#===  BASE LIB  ================================================================
include_directories("/root/thilenius")
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS regex system filesystem REQUIRED)

add_library (
  base
  /root/thilenius/base/arguments.cc
  /root/thilenius/base/auto_update.cc
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

target_link_libraries (
  base
  ${Boost_REGEX_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  crypto
  libcurl
  libgflags
  libgmock
  libgtest
  libz
  ssl
)

add_custom_target(combined ALL
  COMMAND ${CMAKE_AR} rc libcombined.a
  $<TARGET_FILE:base>
  ${Boost_REGEX_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  /usr/lib/x86_64-linux-gnu/libcrypto.a
  $<TARGET_FILE:libcurl>
  $<TARGET_FILE:libgflags>
  $<TARGET_FILE:libgmock>
  $<TARGET_FILE:libgtest>
  $<TARGET_FILE:libz>
  /usr/lib/x86_64-linux-gnu/libssl.a
)

#===  BASE TESTS  ==============================================================
add_executable (
  base_test
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
