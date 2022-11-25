cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(Shared)

set(CMAKE_CXX_STANDARD 20)

add_library(Shared STATIC
        ConstantDefinitions.h
        library.cpp library.h
        Window.cpp Window.h)

find_package(Vulkan REQUIRED)
add_subdirectory(glfw)
include_directories(glfw/include)
target_link_libraries(Shared glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)

target_compile_definitions(Shared PRIVATE WIN_EXPORT)

install(TARGETS Shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
