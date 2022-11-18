cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WindowsImpl)

set(CMAKE_CXX_STANDARD 20)

add_library(WindowsImpl SHARED library.cpp)
