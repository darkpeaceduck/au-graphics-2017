
cmake_minimum_required(VERSION 3.5)
project(deferred)
find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#find_package(GLFW REQUIRED)
find_library(Assimp_library NAMES assimp)
find_library(GLFW_library NAMES glfw)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
