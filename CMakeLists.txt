cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(GlobalWallPepper)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Shared)

if (WIN32)
    add_subdirectory(WindowsImpl)
else ()
endif ()
add_subdirectory(WallPepper)