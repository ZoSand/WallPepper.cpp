cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(WallPepper)

set(CMAKE_CXX_STANDARD 20)

add_executable(WallPepper
        main.cpp)

find_package(Vulkan REQUIRED)

if (WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/WindowsImpl) # include files from libraries
    target_link_libraries(WallPepper WindowsImpl)
else ()
    include_directories(${CMAKE_SOURCE_DIR}/LinuxImpl) # include files from libraries
    target_link_libraries(WallPepper LinuxImpl)
endif ()


add_subdirectory(glfw)
include_directories(glfw/include)
target_link_libraries(WallPepper glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)

install(TARGETS WallPepper
        RUNTIME DESTINATION bin)