cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WindowsImpl)

set(CMAKE_CXX_STANDARD 20)

add_library(WindowsImpl SHARED library.cpp)

include_directories(${CMAKE_SOURCE_DIR}/Shared) # include files from libraries
target_link_libraries(WindowsImpl Shared)
install(TARGETS WindowsImpl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
