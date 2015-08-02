cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install Thrift
set(thrift_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/thrift)
ExternalProject_Add(
    thrift
    PREFIX ${thrift_PREFIX}
    URL http://www.trieuvan.com/apache/thrift/0.9.2/thrift-0.9.2.tar.gz
    URL_MD5 89f63cc4d0100912f4a1f8a9dee63678
    CONFIGURE_COMMAND ${thrift_PREFIX}/src/thrift/configure --enable-shared=no --with-c_glib=no
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
)
include_directories(${thrift_PREFIX}/include)
set(THRIFT_LIBRARIES /usr/local/lib/libthrift.a)

# Create a libthrift target to be used as a dependency by test programs
add_library(libthrift IMPORTED STATIC GLOBAL)
add_dependencies(libthrift thrift)

# Set thrift properties
ExternalProject_Get_Property(thrift source_dir binary_dir)
set_target_properties(libthrift PROPERTIES
    "IMPORTED_LOCATION" "${THRIFT_LIBRARIES}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
