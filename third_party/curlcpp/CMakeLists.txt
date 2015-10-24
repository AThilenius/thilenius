cmake_minimum_required (VERSION 2.6)
include (/root/thilenius/tools/cmake/cmake_macros.txt)
project_guarded (curlcpp)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory (/root/thilenius/third_party/curl
                  /root/thilenius_bin/third_party/curl)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install curlcpp
set(curlcpp_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/curlcpp)
ExternalProject_Add(
    curlcpp
    PREFIX ${curlcpp_PREFIX}
    URL https://github.com/JosephP91/curlcpp/archive/master.zip
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE 1
)
set(curlcpp_LIBRARIES ${curlcpp_PREFIX}/src/curlcpp/src/libcurlcpp.a)

# Create a libcurlcpp target to be used as a dependency by test programs
add_library(libcurlcpp IMPORTED STATIC GLOBAL)
add_dependencies(libcurlcpp curlcpp curl)
set_target_properties(libcurlcpp PROPERTIES
    "IMPORTED_LOCATION" "${curlcpp_LIBRARIES}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
