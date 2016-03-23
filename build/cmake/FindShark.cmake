# This file depends on the value of SHARK_ROOT and it sets the following:
#    Shark_LIBRARIES
#    Shark_INCLUDE_DIRS

find_path(Shark_INCLUDE_DIR
    NAMES "shark/Core/Shark.h"
    PATHS "${SHARK_ROOT}"
    PATH_SUFFIXES "include/shark"
    )

find_library(Shark_LIBRARY_RELEASE
    NAMES shark
    PATHS "${SHARK_ROOT}"
    PATH_SUFFIXES "lib"
    )

find_library(Shark_LIBRARY_DEBUG
    NAMES shark_debug
    PATHS "${SHARK_ROOT}"
    PATH_SUFFIXES "lib"
    )

set(Shark_LIBRARY optimized ${Shark_LIBRARY_RELEASE} debug ${Shark_LIBRARY_DEBUG})
	
# This CMake-supplied script provides standard error handling.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Shark
    FAIL_MESSAGE "Could NOT find Shark. Set SHARK_ROOT to the Shark directory."
    REQUIRED_VARS Shark_LIBRARY Shark_INCLUDE_DIR
    )
    
# SHARK_FOUND is set automatically for us by find_package().
if(SHARK_FOUND)
    set(Shark_INCLUDE_DIRS ${Shark_INCLUDE_DIR})
    set(Shark_LIBRARIES ${Shark_LIBRARY})
endif()

mark_as_advanced(
    Shark_LIBRARY
    Shark_INCLUDE_DIR
)

#unset(Shark_INCLUDE_DIR CACHE)