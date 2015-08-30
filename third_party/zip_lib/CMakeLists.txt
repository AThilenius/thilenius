cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install zip_lib
set(zip_lib_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/zip_lib)
ExternalProject_Add(
    zip_lib
    PREFIX ${zip_lib_PREFIX}
    URL https://bitbucket.org/wbenny/ziplib/get/3e71d7bf92e9.zip
    CONFIGURE_COMMAND ""
    BUILD_COMMAND $(MAKE) CFLAGS+=-static CFLAGS=-fPIC
    INSTALL_COMMAND mv ${zip_lib_PREFIX}/src/zip_lib/Bin/libzip.so
        ${zip_lib_PREFIX}/src/zip_lib/Bin/libzip.a
    BUILD_IN_SOURCE 1
)
set(zip_lib_LIBRARIES ${zip_lib_PREFIX}/src/zip_lib/Bin/libzip.a)

add_library(ziplib IMPORTED STATIC GLOBAL)
add_dependencies(ziplib zip_lib)
set_target_properties(ziplib PROPERTIES
    "IMPORTED_LOCATION" "${zip_lib_LIBRARIES}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
