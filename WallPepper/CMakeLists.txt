cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepper)

set(CMAKE_CXX_STANDARD 20)

add_executable(WallPepper
        main.cpp Application.cpp Application.h)

if (WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/WindowsImpl) # include files from libraries
    target_link_libraries(WallPepper WindowsImpl)
else ()
    include_directories(${CMAKE_SOURCE_DIR}/LinuxImpl) # include files from libraries
    target_link_libraries(WallPepper LinuxImpl)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/Shared)

install(TARGETS WallPepper
        RUNTIME DESTINATION bin)