cmake_minimum_required(VERSION 3.12.0)

project(scone)

# To create a folder hierarchy within Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find dependencies.
# ------------------
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/CMakeModules")

# find flut include path
# find_package(Flut REQUIRED)
find_package(Spot REQUIRED)
find_package(Xo REQUIRED)

option(SCONE_OPENSIM_3 "Support OpenSim 3.3" ON)
option(SCONE_OPENSIM_4 "Support OpenSim 4.0 - EXPERIMENTAL" OFF)

# CMake has the ability to find Qt; we don't need to provide additional files.
find_package(Qt5Widgets)
# http://doc.qt.io/qt-5/cmake-manual.html

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
    add_subdirectory(submodules/vis)
endif()

add_subdirectory(submodules/xo/xo)
add_subdirectory(submodules/xo/test)
add_subdirectory(submodules/spot)

#
# Add targets
#
add_subdirectory(src/sconelib)
add_subdirectory(src/sconecmd)
add_subdirectory(src/sconestudio)
add_subdirectory(src/sconeunittests)

#
# Add tests
#
enable_testing()
add_test( xo_test "${EXECUTABLE_OUTPUT_PATH}/xo_test")
add_test( scone_unit_tests "${EXECUTABLE_OUTPUT_PATH}/sconeunittests")
