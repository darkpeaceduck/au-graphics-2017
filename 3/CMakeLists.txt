
cmake_minimum_required(VERSION 3.5)
project(deferred)
find_package(OpenGL REQUIRED)
find_library(Assimp_library NAMES assimp)
#find_library(GLFW_library NAMES glfw)
find_library(GLFW_library NAMES "glfw3" PATHS lib/glfw/src NO_DEFAULT_PATH)
include_directories("lib/glfw/include")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OPENGL_LIBRARY
            ${GLFW_library}
            ${OPENGL_LIBRARY}
            ${CMAKE_DL_LIBS}
            ${Assimp_library}
            pthread
            X11
            )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_LIBRARY
            ${GLFW_library}
            ${OPENGL_LIBRARY}
            ${CMAKE_DL_LIBS}
            ${Assimp_library}
            )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

add_subdirectory(src)
