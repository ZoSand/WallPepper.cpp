cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepper)

set(CMAKE_CXX_STANDARD 20)

add_executable(WallPepper main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/WindowsImpl) # include files from libraries
target_link_libraries(WallPepper WindowsImpl)

set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CSS_STANDARD_LIBRARIES}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

install(TARGETS WallPepper
        RUNTIME DESTINATION bin)