cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepperGlobal)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Shared)

message(STATUS "DLLs hidden in %USERPROFILE%/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-1/223.7571.59/bin/mingw/bin")

#platform-specific implementations
if (WIN32)
    add_subdirectory(WindowsImpl)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build") #set output dir
else ()
    add_subdirectory(LinuxImpl)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build") #set output dir
endif ()

add_subdirectory(WallPepper)