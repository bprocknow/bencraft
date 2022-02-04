find_library(OPENGL_LIBRARY NAMES gl GL))
find_path(OPENGL_INCLUDE_DIR NAMES GL/gl.h GL/glext.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GL DEFAULT_MSG OPENGL_LIBRARY OPENGL_INCLUDE_DIR)
mark_as_advanced(OPENGL_LIBRARY OPENGL_INCLUDE_DIR)

