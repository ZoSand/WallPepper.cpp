cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(LinuxImpl)

set(CMAKE_CXX_STANDARD 20)

#add_library(WindowsImpl library.cpp)
add_library(LinuxImpl SHARED library.cpp library.h Wallpaper.cpp Wallpaper.h)

include_directories(${CMAKE_SOURCE_DIR}/Shared) # include files from libraries

target_link_libraries(LinuxImpl Shared)
target_compile_definitions(LinuxImpl PRIVATE LINUX_EXPORT)

install(TARGETS LinuxImpl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
