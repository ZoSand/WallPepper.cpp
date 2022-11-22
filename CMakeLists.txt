cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepperGlobal)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Shared)

message(WARNING "DLLs hidden in %USERPROFILE%/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-1/223.7571.59/bin/mingw/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build") #set output dir

#platform-specific implementations
if (WIN32)
    add_subdirectory(WindowsImpl)
else ()
    add_subdirectory(LinuxImpl)
endif ()

add_subdirectory(WallPepper)