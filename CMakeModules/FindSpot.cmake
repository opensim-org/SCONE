# Find FLUT headers
# TODO: use a better path estimate!
find_path(SPOT_INCLUDE_DIR
	NAMES "spot/optimizer.h"
	PATHS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/submodules/spot
	)

# This CMake-supplied script provides standard error handling.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLUT
    FAIL_MESSAGE "Could NOT find FLUT. Set FLUT_INCLUDE_DIR to the FLUT directory."
    REQUIRED_VARS SPOT_INCLUDE_DIR
    )
    
# SPOT_FOUND is set automatically for us by find_package().
