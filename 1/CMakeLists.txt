
cmake_minimum_required(VERSION 3.5)
project(deferred)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_library(GLUI_LIB NAMES "glui" PATHS lib/glui/lib NO_DEFAULT_PATH)
find_library(GLU_LIB NAMES GLU)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories( "lib/glui/include" )
add_subdirectory(src)
