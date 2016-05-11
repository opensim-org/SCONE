cmake_minimum_required(VERSION 2.8.11)
# 2.8.11 is required for how we use Qt5.

project(scone)

# To create a folder hierarchy within Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# User options.
# -------------
set(SHARK_ROOT "" CACHE PATH "Installation directory of shark.")

# Find dependencies.
# ------------------
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/CMakeModules")

# find flut include path
find_package(Flut REQUIRED)

# Shark 3.0 provides a SharkConfig.cmake.
find_package(Shark 3.0 REQUIRED PATHS "${SHARK_ROOT}")

# Link with boost's static libraries.
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS 
	filesystem system thread date_time chrono timer serialization unit_test_framework)
if(UNIX)
	ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
endif()

# This uses the FindOpenSim.cmake file in build/cmake.
find_package(OpenSim 3.3 REQUIRED)

# CMake has the ability to find Qt; we don't need to provide additional files.
find_package(Qt5Widgets)
# http://doc.qt.io/qt-5/cmake-manual.html

# Incorporate dependencies.
# -------------------------
# Reports automatic linking info with Visual Studio.
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
if(WIN32)
	add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK)
endif()
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS})
include_directories(${FLUT_INCLUDE_DIRS})
include(${SHARK_USE_FILE}) # UseShark.cmake file actually handles the includes

# Add sconelib include folder to all projects
include_directories(${CMAKE_SOURCE_DIR}/scone/sconelib)

# Compiler flags.
# ---------------
# Must compile with C++11 with gcc/clang.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
   ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(APPLE)
        if(XCODE)
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
            if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
            endif()
        endif()
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
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
add_subdirectory(submodules/flut)

#
# Add single sconelib target
#
add_subdirectory(scone/sconelib)

#
# Add individual libs
#
#add_subdirectory(scone/core)
#add_subdirectory(scone/sim)
#add_subdirectory(scone/sim/simbody)
#add_subdirectory(scone/opt)
#add_subdirectory(scone/cs)
#add_subdirectory(scone/vis)

#
# Add target executables
#
add_subdirectory(scone/sconemanager)
add_subdirectory(scone/sconeopt)
add_subdirectory(scone/sconestudio)
add_subdirectory(scone/sconeunittests)
add_subdirectory(scone/test)
