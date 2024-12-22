cmake_minimum_required(VERSION 3.10)
project(03-Terrain)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/framework)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glm)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glfw/include)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib/libglfw3.a
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
)

add_definitions(-DWINDOW_PUT_PIXEL)

add_executable(03-Terrain
        03-Terrain/main.cpp
        03-Terrain/Terrain.cpp
        03-Terrain/Pseudorandom.h
        framework/Window.cpp
)

target_link_libraries(03-Terrain PRIVATE glad glfw opengl32)
