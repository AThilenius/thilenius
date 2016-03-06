cmake_minimum_required (VERSION 2.6)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (
  thilenius_hw3
  fasta_file.cc
  main.cc
  needleman_wunsch.cc
)
