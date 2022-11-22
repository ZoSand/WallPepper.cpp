cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepper)

set(CMAKE_CXX_STANDARD 20)

add_executable(WallPepper
        main.cpp)

find_package(Vulkan REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/WindowsImpl) # include files from libraries
target_link_libraries(WallPepper WindowsImpl Vulkan::Vulkan)

add_subdirectory(glfw)
include_directories(glfw/include)
target_link_libraries(WallPepper glfw ${GLFW_LIBRARIES})

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
endif()

install(TARGETS WallPepper
        RUNTIME DESTINATION bin)