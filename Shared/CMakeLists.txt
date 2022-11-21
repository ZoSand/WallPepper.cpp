cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(Shared)

set(CMAKE_CXX_STANDARD 20)

add_library(Shared library.cpp)

install(TARGETS Shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
