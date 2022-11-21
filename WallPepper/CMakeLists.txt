cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepper)

set(CMAKE_CXX_STANDARD 20)

add_executable(WallPepper main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/WindowImpl) # include files from libraries
target_link_libraries(WallPepper WindowImpl)

install(TARGETS WallPepper RUNTIME DESTINATION bin)