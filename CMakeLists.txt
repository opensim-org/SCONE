cmake_minimum_required(VERSION 3.10.0)
# 2.8.11 is required for how we use Qt5.

project(scone)

# To create a folder hierarchy within Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies.
# ------------------
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/CMakeModules")

# find flut include path
# find_package(Flut REQUIRED)
find_package(Spot REQUIRED)
find_package(Xo REQUIRED)

option(SCONE_OPENSIM_3 "Support OpenSim 3.3" ON)
option(SCONE_OPENSIM_4 "Support OpenSim 4.0" OFF)
if(SCONE_OPENSIM_3)
    # This uses the FindOpenSim.cmake file in build/cmake.
    find_package(OpenSim 3.3 REQUIRED MODULE)
endif()
if(SCONE_OPENSIM_4)
    find_package(OpenSim 4.0 REQUIRED CONFIG)
endif()
if(SCONE_OPENSIM_3 AND SCONE_OPENSIM_4)
    message(FATAL_ERROR "Cannot currently use both OpenSim 3 and 4.")
endif()

# CMake has the ability to find Qt; we don't need to provide additional files.
find_package(Qt5Widgets)
# http://doc.qt.io/qt-5/cmake-manual.html

# include folders
if(SCONE_OPENSIM_3)
    include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS})
endif()
include_directories(${XO_INCLUDE_DIR})
include_directories(${SPOT_INCLUDE_DIR})

# Add sconelib include folder to all projects
include_directories(${CMAKE_SOURCE_DIR}/src/sconelib)

# Add TCLAP
include_directories(${CMAKE_SOURCE_DIR}/contrib/tclap-1.2.1/include)

if (MSVC)
	add_compile_options("/MP")
endif()

# Various settings
# ----------------
# Place build products (libraries, executables) in root
# binary (build) directory. Otherwise, they get scattered around
# the build directory and so the dll's aren't next to the executables.
get_filename_component(CONFIG_NAME "${CMAKE_BINARY_DIR}" NAME)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CONFIG_NAME}")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CONFIG_NAME}")

# Silence CMake warning on OSX.
set(CMAKE_MACOSX_RPATH ON)

# Process source code.
# --------------------
# Only build visualizer if Qt5 was found
if(${Qt5Widgets_FOUND})
    add_subdirectory(submodules/simvis)
endif()

add_subdirectory(submodules/xo)
add_subdirectory(submodules/spot)

#
# Add targets
#
add_subdirectory(src/sconelib)
add_subdirectory(src/sconecmd)
add_subdirectory(src/sconestudio)
add_subdirectory(src/sconeunittests)
