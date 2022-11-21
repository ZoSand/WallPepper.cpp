cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepperGlobal)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Shared)

#platform-specific implementations
if (WIN32)
    add_subdirectory(WindowsImpl)
    #WARNING: DISABLE THIS LINE BEFORE RELEASE
    message(WARNING "DISABLE CMAKE_RUNTIME_OUTPUT_DIRECTORY BEFORE RELEASE")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug/WindowsImpl")
else ()
    add_subdirectory(LinuxImpl)
    #WARNING: DISABLE THIS LINE BEFORE RELEASE
    message(WARNING "DISABLE CMAKE_RUNTIME_OUTPUT_DIRECTORY BEFORE RELEASE")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug/LinuxImpl")
endif ()

add_subdirectory(WallPepper)