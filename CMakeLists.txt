cmake_minimum_required(VERSION 3.12.0)

project(scone)

# To create a folder hierarchy within Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# compilation database for completion on Linux
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies.
# ------------------
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/CMakeModules")

option(SCONE_OPENSIM_3 "Support OpenSim 3.3" ON)
option(SCONE_OPENSIM_4 "Support OpenSim 4.0 - EXPERIMENTAL" OFF)
option(SCONE_LUA "Support for lua scripting" OFF)
option(SCONE_HYFYDY "Support Hyfydy - EXPERIMENTAL" OFF)
option(SCONE_ENABLE_PROFILER "Enable SCONE profiler" ON)

# CMake has the ability to find Qt; we don't need to provide additional files.
find_package(Qt5Widgets)
# http://doc.qt.io/qt-5/cmake-manual.html

# Various settings
# ----------------
# Place build products (libraries, executables) in root
# binary (build) directory. Otherwise, they get scattered around
# the build directory and so the dll's aren't next to the executables.
#get_filename_component(CONFIG_NAME "${CMAKE_BINARY_DIR}" NAME)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CONFIG_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set `rpath` when building for Linux/OSX
#
#  This enables a portable install of SCONE, where the user can just
#  unpack the executables + libraries *somewhere* (not known at
#  compile-time) and execute any SCONE binary.
if (UNIX OR APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

    # This is necessary because newer linkers "helpfully" interpret
    # `rpath` as `RUNPATH`, which does not transistively apply the
    # rpath to all dependencies (which means that any of SCONE's
    # transitive library dependencies will fail to be located at
    # runtime)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")
endif()

# Process source code.
# --------------------
# Only build visualizer if Qt5 was found
if(${Qt5Widgets_FOUND})
    add_subdirectory(submodules/vis)
endif()

# Build lua if SCONE_LUA is set
if (SCONE_LUA)
	add_subdirectory(contrib/lua-5.3.5)
endif()

#
# Add submodules
#
add_subdirectory(submodules/xo)
add_subdirectory(submodules/spot)

#
# Add targets
#
add_subdirectory(src/sconelib)
add_subdirectory(src/sconecmd)
add_subdirectory(src/sconestudio)
add_subdirectory(src/sconeunittests)

if (SCONE_HYFYDY)
	find_path( SCONE_HYFYDY_DIR "sconehfd/ModelHfd.h" HINTS "../sconehfd" )
	add_subdirectory("${SCONE_HYFYDY_DIR}" "${CMAKE_BINARY_DIR}/sconehfd")
endif()

#
# Add tests
#
enable_testing()
if (XO_TEST_ENABLED)
	add_test( xo_test "${EXECUTABLE_OUTPUT_PATH}/xo_test")
endif()
add_test( scone_unit_tests "${EXECUTABLE_OUTPUT_PATH}/sconeunittests")
