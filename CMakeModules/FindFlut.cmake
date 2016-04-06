# Find FLUT headers
# TODO: use a better path estimate!
find_path(FLUT_INCLUDE_DIR
    NAMES flut/system/types.hpp
    PATHS ${CMAKE_SOURCE_DIR}/submodules/flut
   )

# This CMake-supplied script provides standard error handling.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLUT
    FAIL_MESSAGE "Could NOT find FLUT. Set FLUT_INCLUDE_DIR to the FLUT directory."
    REQUIRED_VARS FLUT_INCLUDE_DIR
    )
    
# SHARK_FOUND is set automatically for us by find_package().
if(FLUT_FOUND)
    set(FLUT_INCLUDE_DIRS ${FLUT_INCLUDE_DIR})
endif()
