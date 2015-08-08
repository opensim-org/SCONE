# FindOpenSim.cmake
# =================
#
# What is this file? What is it part of?
# --------------------------------------
# This is a CMake "find" module that will try to find the OpenSim
# musculoskeletal modeling and simulation package installed somewhere on your
# computer. OpenSim is part of the SimTK biosimulation toolkit. For more
# information, see https://simtk.org/home/opensim.
#
#
# How do I incorporate it into my own CMake project?
# ----------------------------------------------------
# To use this file in a find_packages() command from your own CMakeLists.txt
# file, make sure it is in a directory contained in the CMAKE_MODULE_PATH. You
# can add a directory to that path with a line like this:
#
#   list(APPEND CMAKE_MODULE_PATH "path/to/this/file")
#
# It is common for Find*.cmake files to be placed in a 'cmake' folder in the
# root of your project. Then, the line above would become:
#
#   list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#
#
# How do I benefit from using this file in my own project?
# --------------------------------------------------------
# To 'run' this file in your CMakeLists.txt, include this line:
#   
#   find_package(OpenSim REQUIRED)
#
# To use the headers and libraries from OpenSim in your own targets:
#
#   include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS})
#   add_executable(myClientApplication ${CLIENT_SOURCE} ${CLIENT_HEADERS})
#   target_link_libraries(myClientApplication ${OPENSIMSIMBODY_LIBRARIES})
#
# You can omit the 'REQUIRED' above if OpenSim isn't actually required for your
# project. If, in this case, you want to check if OpenSim was found, check the
# value of OPENSIM_FOUND.
#
#
# What variables does this script define?
# ---------------------------------------
#   OPENSIM_FOUND - If OpenSim libraries and headers were found.
#   OPENSIM_ROOT_DIR - the OpenSim installation directory.
#   OPENSIM_INCLUDE_DIRS - location of OpenSim/OpenSim.h.
#   OPENSIMSIMBODY_INCLUDE_DIRS - list of two directories:
#       location of OpenSim/OpenSim.h
#       location of Simbody.h
#   OPENSIM_LIB_DIR - location of libosim*.{a,so,dylib}, etc. This directory
#       contains Simbody libraries as well. On Windows, this is the same as
#       OPENSIM_BIN_DIR.
#   OPENSIM_BIN_DIR - location of executables/OpenSim tools.
#   OPENSIM_LIBRARIES - suitable for target_link_libraries(). Contains the
#       libraries with 'osim' in their name.
#   OPENSIMSIMBODY_LIBRARIES - suitable for target_link_libraries(). Contains
#       libraries with 'osim' or 'SimTK' in their name.
#
# How does the script work?
# -------------------------
# To find OpenSim on your computer, we look for the following environment
# variables:
#
#   OPENSIM_HOME
#
# If the variables does not exist, we check common locations. If the
# script struggles, or finds the wrong version, you can direct it by setting
# the CMake variable:
#
#   OPENSIM_INSTALL_DIR
#
# The value of OPENSIM_INSTALL_DIR has the highest priority.
#
#
# OpenSim depends on Simbody, right? How does that work?
# ------------------------------------------------------
# Currently (18 Oct 2013), OpenSim distributes a version of Simbody that is
# consistent with the distribution of OpenSim. Most users will use this version
# of Simbody (probably won't have a separate installation of Simbody), and thus
# will use the variables that start with OPENSIMSIMBODY_*. If you'd like to use
# a separate installation of Simbody, use the OPENSIM_* variables above instead.
# Then, you can find_package(Simbody) using Simbody's
# FindSimbody.cmake or SimbodyConfig.cmake scripts.
#
#
# TODO
# ----
# - *MOST IMPORTANT* In find_path(... PATH_SUFFIXES), detect OpenSim x.x (any
#       version number).
# - Deal with the library NameSpace.
# - Version selection. Right now, the version argument to find_package is
#   ignored.
# - Static libraries.
# - Should OPENSIM_LIB_DIR point to bin on Windows?
#
# See this website to see how these scripts should be written:
# www.cmake.org/Wiki/CMake:How_To_Find_Libraries
#
# The basic gist of the script below is that we first try to set *_INCLUDE_DIR
# and *_LIBRARY variables. If we DO find OpenSim, then we also set the
# *_INCLUDE_DIRS and *_LIBRARIES variables that the client is expected to use.

cmake_minimum_required(VERSION 2.8)

# Search for an OpenSim installation.
# -----------------------------------
set(OPENSIM_HOME "$ENV{OPENSIM_HOME}")
if(OPENSIM_INSTALL_DIR AND NOT "${OPENSIM_INSTALL_DIR}" STREQUAL "")
    set(OPENSIM_SEARCH_PATHS "${OPENSIM_INSTALL_DIR}")
elseif(OPENSIM_HOME)
    set(OPENSIM_SEARCH_PATHS "${OPENSIM_HOME}")
else()
    # Hunt for the installation.
    set(OPENSIM_SEARCH_PATHS)

    # Mac, Linux, Cygwin.
    if(UNIX)
        list(APPEND OPENSIM_SEARCH_PATHS /usr/local)
        # Unlikely (for when OpenSim is distributed through distro package
        # managers):
        list(APPEND OPENSIM_SEARCH_PATHS /usr)
    endif()

    if(APPLE)
        list(APPEND OPENSIM_SEARCH_PATHS /Developer)
    endif()

    # Windows 32 and 64 bit, Cygwin.
    if(WIN32)
        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            # 64 bit target on 64-bit Windows.
            set(PROGFILE_DIR "$ENV{ProgramW6432}")
        else()
            # Target is 32-bit on 64-bit Windows.
            set(PROGFILE_DIR "$ENV{ProgramFiles(x86)}")
            if(NOT PROGFILE_DIR)
                # On 32-bit Windows.
                set(PROGFILE_DIR "$ENV{ProgramFiles}")
            endif()
        endif()
        list(APPEND Simbody_SEARCH_PATHS ${PROGFILE_DIR})
    endif()
endif()


# OPENSIM_INCLUDE_DIR and OPENSIMSIMBODY_INCLUDE_DIRS
# ---------------------------------------------------
# We find OpenSim by finding sdk/include/OpenSim/OpenSim.h.
set(OPENSIM_INCLUDE_DIR_DOC
    "The location of OpenSim/OpenSim.h and all OpenSim headers.")
set(OPENSIMSIMBODY_INCLUDE_DIR_DOC
    "The locations of OpenSim and Simbody headers.")

find_path(OPENSIM_INCLUDE_DIR
    NAMES "OpenSim/OpenSim.h"
    PATHS ${OPENSIM_SEARCH_PATHS}
    PATH_SUFFIXES "sdk/include" "opensim/sdk/include" "OpenSim/sdk/include"
    DOC ${OPENSIM_INCLUDE_DIR_DOC}
    )

# This change is necessary for Simbody 3.4 and beyond, and is incompatible
# with Simbody 3.3 and below.
set(OPENSIM_SIMBODY_INCLUDE_RELPATH "include")

set(OPENSIMSIMBODY_INCLUDE_DIR
    ${OPENSIM_INCLUDE_DIR}
    ${OPENSIM_INCLUDE_DIR}/SimTK/${OPENSIM_SIMBODY_INCLUDE_RELPATH}
    )


# OPENSIM_ROOT_DIR
# ----------------
# Back out the root installation directory.
get_filename_component(OPENSIM_SDK_DIR "${OPENSIM_INCLUDE_DIR}" PATH)
get_filename_component(OPENSIM_ROOT_DIR "${OPENSIM_SDK_DIR}" PATH)

# OPENSIM_LIB_DIR and OPENSIM_BIN_DIR
# -----------------------------------
if(WIN32)
    set(OPENSIM_PLATFORM_LIB_RELPATH "sdk/lib")
else()
    set(OPENSIM_PLATFORM_LIB_RELPATH "lib")
endif()
set(OPENSIM_LIB_DIR ${OPENSIM_ROOT_DIR}/${OPENSIM_PLATFORM_LIB_RELPATH})
set(OPENSIM_BIN_DIR ${OPENSIM_ROOT_DIR}/bin)


# OPENSIM_LIBRARIES and OPENSIMSIMBODY_LIBRARIES
# ----------------------------------------------
set(OPENSIM_LIBRARIES_DOC "Suitable for target_link_libraries(). Contains only
    the libraries with 'osim' in their name.")
set(OPENSIMSIMBODY_LIBRARIES_DOC "Suitable for target_link_libraries().
    Contains libraries with either 'osim' or 'SimTK' in their name.")

# This variables are for our purposes only; its name comes from convention:
set(OPENSIM_LIBRARY)

set(OPENSIM_LIBRARY_LIST
    osimCommon osimSimulation osimAnalyses osimActuators osimTools)
set(SIMBODY_LIBRARY_LIST SimTKcommon SimTKmath SimTKsimbody
        OpenSim_SimTKcommon OpenSim_SimTKmath OpenSim_SimTKsimbody)

foreach(LIB_NAME IN LISTS OPENSIM_LIBRARY_LIST)
    find_library(FOUND_LIB NAMES ${LIB_NAME}
        PATHS "${OPENSIM_LIB_DIR}"
        NO_DEFAULT_PATH)
    if(FOUND_LIB)
        list(APPEND OPENSIM_LIBRARY optimized ${FOUND_LIB})
    endif()
    unset(FOUND_LIB CACHE)

    find_library(FOUND_LIB NAMES ${LIB_NAME}_d
        PATHS "${OPENSIM_LIB_DIR}"
        NO_DEFAULT_PATH)
    if(FOUND_LIB)
        list(APPEND OPENSIM_LIBRARY debug ${FOUND_LIB}_d)
    endif()
    unset(FOUND_LIB CACHE)
endforeach()

# Start off this list of libraries with the OpenSim libraries.
set(OPENSIMSIMBODY_LIBRARY ${OPENSIM_LIBRARY})

foreach(LIB_NAME IN LISTS SIMBODY_LIBRARY_LIST)
    find_library(FOUND_LIB NAMES ${LIB_NAME}
        PATHS "${OPENSIM_LIB_DIR}"
        NO_DEFAULT_PATH)
    if(FOUND_LIB)
        list(APPEND OPENSIMSIMBODY_LIBRARY optimized ${FOUND_LIB})
    endif()
    unset(FOUND_LIB CACHE)

    find_library(FOUND_LIB NAMES ${LIB_NAME}_d
        PATHS "${OPENSIM_LIB_DIR}"
        NO_DEFAULT_PATH)
    if(FOUND_LIB)
        list(APPEND OPENSIMSIMBODY_LIBRARY debug ${FOUND_LIB}_d)
    endif()
    unset(FOUND_LIB CACHE)
endforeach()


# Wrap up
# -------
set(OPENSIM_INSTALL_DIR "${OPENSIM_ROOT_DIR}"
    CACHE PATH "The OpenSim installation directory." FORCE)

# This CMake-supplied script provides standard error handling.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenSim
    "
    Could NOT find OpenSim. Try setting OPENSIM_INSTALL_DIR, or 
    create an environment variable OPENSIM_HOME."
    OPENSIM_INCLUDE_DIR)

# OPENSIM_FOUND is set automatically for us by find_package().
if(OPENSIM_FOUND)
    set(OPENSIM_INCLUDE_DIRS ${OPENSIM_INCLUDE_DIR})
    set(OPENSIMSIMBODY_INCLUDE_DIRS ${OPENSIMSIMBODY_INCLUDE_DIR})
    set(OPENSIM_LIBRARIES ${OPENSIM_LIBRARY})
    set(OPENSIMSIMBODY_LIBRARIES ${OPENSIMSIMBODY_LIBRARY})
endif()

mark_as_advanced(
    OPENSIM_ROOT_DIR
    OPENSIM_INCLUDE_DIR
    OPENSIMSIMBODY_INCLUDE_DIR
    OPENSIM_BIN_DIR
    OPENSIM_LIB_DIR
    OPENSIM_LIBRARY
    OPENSIMSIMBODY_LIBRARY
)

# The following allows us to change the OpenSim installation we use.
unset(OPENSIM_INCLUDE_DIR CACHE)
