cmake_minimum_required (VERSION 2.6)
project (base)

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
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

#===  GOOGLE MOCK  =============================================================
ExternalProject_Add(
    gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
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

#===  BASE  ====================================================================
include_directories("/root/thilenius")

add_library (
  base
  /root/thilenius/base/file.cc
  /root/thilenius/base/log.cc
  /root/thilenius/base/path.cc
  /root/thilenius/base/string.cc
)

target_link_libraries (
  base
  libgflags
  libgtest
  libgmock
  libglog
)
