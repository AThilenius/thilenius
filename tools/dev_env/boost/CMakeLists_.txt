cmake_minimum_required (VERSION 2.6)

# Enable ExternalProject CMake module
include(ExternalProject)

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
  INSTALL_COMMAND ""
  INSTALL_DIR /usr
)
