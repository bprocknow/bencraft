find_library(EGL_LIBRARY NAMES egl EGL)
find_path(EGL_INCLUDE_DIR NAMES EGL/egl.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY EGL_INCLUDE_DIR)
mark_as_advanced(EGL_LIBRARY EGL_INCLUDE_DIR)
