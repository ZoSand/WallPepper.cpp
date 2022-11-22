cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(Shared)

set(CMAKE_CXX_STANDARD 20)

add_library(Shared
        ConstantDefinitions.h
        library.cpp library.h
        Window.cpp Window.h)

target_compile_definitions(Shared PRIVATE WIN_EXPORT)

install(TARGETS Shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
