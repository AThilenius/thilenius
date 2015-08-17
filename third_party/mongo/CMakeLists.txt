cmake_minimum_required (VERSION 2.6)

find_package (Boost
  REQUIRED
  COMPONENTS filesystem program_options system thread
)
include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

set (MONGOCLIENT_SCONS_FLAGS
  ${MONGOCLIENT_SCONS_FLAGS}
  --warn=no-missing-sconscript
  --cache
  --cpppath=${Boost_INCLUDE_DIRS}
  --libpath=${Boost_LIBRARY_DIRS}
  --warn=no-all
  --cc=${CMAKE_C_COMPILER}
  --cxx=${CMAKE_CXX_COMPILER}
  --c++11=on
  --prefix=/usr
)

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
  UPDATE_COMMAND ""
  BUILD_COMMAND scons ${MONGOCLIENT_SCONS_FLAGS} install
  INSTALL_COMMAND ""
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
