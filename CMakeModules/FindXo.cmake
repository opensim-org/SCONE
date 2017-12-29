# Find XO headers
# TODO: use a better path estimate!
find_path(XO_INCLUDE_DIR
    NAMES xo/system/platform.h
    PATHS ${CMAKE_SOURCE_DIR}/submodules/xo
   )

# This CMake-supplied script provides standard error handling.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XO
    FAIL_MESSAGE "Could NOT find XO. Set XO_INCLUDE_DIR to the XO directory."
    REQUIRED_VARS XO_INCLUDE_DIR
    )
    
# XO_FOUND is set automatically for us by find_package().
if(XO_FOUND)
    set(XO_INCLUDE_DIRS ${XO_INCLUDE_DIR})
endif()
