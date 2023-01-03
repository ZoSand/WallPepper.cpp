cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(Shared)

set(CMAKE_CXX_STANDARD 20)

add_library(Shared STATIC
        ConstantDefinitions.h
        library.cpp library.h
        Window.cpp Window.h EngineBase.h VulkanEngine.cpp VulkanEngine.h)

find_package(Vulkan REQUIRED)
add_subdirectory(glfw)
add_subdirectory(glm)
include_directories(glfw/include)
include_directories(glm)
target_link_libraries(Shared glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)

target_compile_definitions(Shared PRIVATE WIN_EXPORT)

install(TARGETS Shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
