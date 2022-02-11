find_library(GLESv2_LIBRARY NAMES GLESv2 GLESv2)
find_path(GLESv2_INCLUDE_DIR NAMES GLES2/gl2.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLESv2 DEFAULT_MSG GLESv2_LIBRARY GLESv2_INCLUDE_DIR)
mark_as_advanced(GLESv2_LIBRARY GLESv2_INCLUDE_DIR)

