cmake_minimum_required (VERSION 2.6)

add_subdirectory (/root/thilenius/third_party/curl
                  /root/thilenius_bin/third_party/curl)

include (ExternalProject)

# Download and install MongoCXX Driver
set(mongo_cxx_drive_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/mongo_cxx_driver)
ExternalProject_Add(
  mongo_cxx_driver
  PREFIX ${mongo_cxx_drive_PREFIX}
  GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
  GIT_TAG legacy
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND scons ${mongo_cxx_drive_PREFIX}-prefix/src/mongo_cxx_driver --cache --prefix=/usr install || echo "Failed to build, continuing anyway."
  BUILD_IN_SOURCE 1
  DEPENDS
    libcurl
)

add_library(libmongoclient IMPORTED STATIC GLOBAL)
add_dependencies(
  libmongoclient
  mongo_cxx_driver
)

ExternalProject_Get_Property(mongo_cxx_driver source_dir binary_dir)
set_target_properties(libmongoclient PROPERTIES
    "IMPORTED_LOCATION" "/usr/lib/libmongoclient.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
