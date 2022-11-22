cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WindowsImpl)

set(CMAKE_CXX_STANDARD 20)

#add_library(WindowsImpl library.cpp)
add_library(WindowsImpl SHARED
        library.cpp library.h
        Wallpaper.cpp Wallpaper.h)

include_directories(${CMAKE_SOURCE_DIR}/Shared) # include files from libraries

target_link_libraries(WindowsImpl Shared)
target_compile_definitions(WindowsImpl PRIVATE WIN_EXPORT)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build")

install(TARGETS WindowsImpl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
