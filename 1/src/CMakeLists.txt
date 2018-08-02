include_directories(.)
add_executable(deferred sdr.c image_ppm.c  mbrot.cpp )
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} )
target_link_libraries(deferred ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLUI_LIB} ${GLU_LIB})