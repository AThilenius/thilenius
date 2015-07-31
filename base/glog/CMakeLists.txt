cmake_minimum_required (VERSION 2.6)

# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleLog
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
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
