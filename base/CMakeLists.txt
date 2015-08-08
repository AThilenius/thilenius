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

#===  GOOGLE LOG  ==============================================================
ExternalProject_Add(
    glog
    URL https://github.com/emzeat/google-glog/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glog
    DEPENDS gflags
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libglog target
add_library(libglog IMPORTED STATIC GLOBAL)
add_dependencies(libglog glog)

# Set glog properties
ExternalProject_Get_Property(glog source_dir binary_dir)
set_target_properties(libglog PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgoogle-glog.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

#===  BASE LIB  ================================================================
include_directories("/root/thilenius")

add_library (
  base
  /root/thilenius/base/directory.cc
  /root/thilenius/base/file.cc
  /root/thilenius/base/log.cc
  /root/thilenius/base/path.cc
  /root/thilenius/base/string.cc
  /root/thilenius/base/text_file.cc
)

target_link_libraries (
  base
  boost_filesystem
  boost_system
  libgflags
  libglog
  libgmock
  libgtest
)

#===  BASE LIB  ================================================================
add_executable (
  base_test
  /root/thilenius/base/directory_test.cc
  /root/thilenius/base/file_test.cc
  /root/thilenius/base/path_test.cc
  /root/thilenius/base/string_test.cc
)

target_link_libraries (
  base_test
  base
  libgtest_main
)
