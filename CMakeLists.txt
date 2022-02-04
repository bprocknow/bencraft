cmake_minimum_required(VERSION 3.16.3)
project(bencraft)

file (GLOB_RECURSE sources ./*.c ./*.h)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" FORCE)
message(${CMAKE_MODULE_PATH})

find_path(X11_INCLUDE_DIR NAMES "X11/Xlib.h" "X11/Xatom.h" "X11/Xutil.h")
find_path(EGL_INCLUDE_DIR NAMES "EGL/egl.h")
find_path(OPENGL_INCLUDE_PATH NAMES "GL/gl.h" "GL/glext.h")
find_path(GLES_INCLUDE_PATH NAMES "GLES/gl2.h")
#find_path(OPENGL_INCLUDE_PATH NAMES "GL/glew.h")

add_executable(bencraft ${sources})

target_compile_options(bencraft PUBLIC -Wall -Wextra -DDEBUG)

set_property(TARGET bencraft PROPERTY C_STANDARD 99)

find_package(OpenGL REQUIRED)
find_package(EGL REQUIRED)
find_package(X11 REQUIRED)
find_package(GLESv2 REQUIRED)

add_library(dynGL SHARED IMPORTED)
set_target_properties(dynGL PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libGL.so)

target_link_libraries(bencraft dynGL ${GLESv2_LIBRARY} ${OPENGL_LIBRARY} ${EGL_LIBRARY} ${DRM_LIBRARY} ${X11_LIBRARIES})
