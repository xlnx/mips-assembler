cmake_minimum_required(VERSION 3.0)
project(mipsa)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "-pg -g")

file(GLOB MIPSA_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
include_directories(${MIPSA_INCLUDE_DIRS})

add_executable("mipsa" "${CMAKE_SOURCE_DIR}/src/mipsa.cpp")
add_executable("mipse" "${CMAKE_SOURCE_DIR}/src/mipse.cpp")