cmake_minimum_required(VERSION 2.4)

project(dlp CXX)

set(CXX_STANDARD 17)

add_executable(dlp
    src/main.cpp
    src/ann.cpp
    src/cg.cpp)

