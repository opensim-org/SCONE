set(MODEL_OPENSIM_4_FILES
	BodyOpenSim4.cpp
	BodyOpenSim4.h
	ContactForceOpenSim4.h
	ContactForceOpenSim4.cpp
	DofOpenSim4.cpp
	DofOpenSim4.h
	JointOpenSim4.cpp
	JointOpenSim4.h
	ModelOpenSim4.cpp
	ModelOpenSim4.h
	MuscleOpenSim4.cpp
	MuscleOpenSim4.h
	ConstantForce.cpp
	ConstantForce.h
	simbody_tools.h
	platform.h
	sconeopensim4.h
	sconeopensim4.cpp
	StateComponentOpenSim4.h
	StateComponentOpenSim4.cpp
)

find_package(OpenSim 4.0 REQUIRED CONFIG) # This uses the FindOpenSim.cmake file in build/cmake.

add_library( sconeopensim4 SHARED ${MODEL_OPENSIM_4_FILES} )
target_link_libraries( sconeopensim4 sconelib osimTools )

# Require C++17 standard
set_target_properties(sconeopensim4 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_precompile_headers(sconeopensim4 PRIVATE <string> <vector> <algorithm> <memory> <limits> <fstream> <OpenSim/OpenSim.h>)
	file (GLOB_RECURSE PRECOMPILED_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cmake_pch.*)
	source_group("CMakePCH" FILES ${PRECOMPILED_HEADER_FILES})

	target_compile_definitions( sconeopensim4 PRIVATE SCONE_MODEL_OPENSIM_4_EXPORTS )
	target_compile_definitions( sconeopensim4 PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS )
	target_compile_definitions( sconeopensim4 PRIVATE $<$<BOOL:${SCONE_ENABLE_PROFILER}>:SCONE_ENABLE_XO_PROFILING> )
	target_compile_options( sconeopensim4 PRIVATE "/MP" ) # multithreaded compilation on MSVC
endif()

source_group("" FILES ${MODEL_OPENSIM_4_FILES})
