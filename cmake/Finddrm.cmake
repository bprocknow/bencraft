find_library(DRM_LIBRARY NAMES drm DRM)
find_path(DRM_INCLUDE_DIR NAMES DRM/drm.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DRM DEFAULT_MSG DRM_LIBRARY DRM_INCLUDE_DIR)
mark_as_advanced(DRM_LIBRARY DRM_INCLUDE_DIR)
