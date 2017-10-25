cmake_minimum_required(VERSION 2.4)

project(cg CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/ann)
include_directories(${CMAKE_SOURCE_DIR}/src/cg)
include_directories(${CMAKE_SOURCE_DIR}/src/ocr)
message(${CMAKE_SOURCE_DIR}/ocr)

add_subdirectory(src)

