set(MODEL_OPENSIM_3_FILES
	BodyOpenSim3.cpp
	BodyOpenSim3.h
	ContactForceOpenSim3.h
	ContactForceOpenSim3.cpp
	DofOpenSim3.cpp
	DofOpenSim3.h
	JointOpenSim3.cpp
	JointOpenSim3.h
	ModelOpenSim3.cpp
	ModelOpenSim3.h
	MuscleOpenSim3.cpp
	MuscleOpenSim3.h
	ConstantForce.cpp
	ConstantForce.h
	simbody_tools.h
	platform.h
	sconeopensim3.h
	sconeopensim3.cpp
	StateComponentOpenSim3.h
	StateComponentOpenSim3.cpp
)

find_package( OpenSim 3.3 REQUIRED MODULE )

add_library( sconeopensim3 SHARED ${MODEL_OPENSIM_3_FILES} )

target_include_directories( sconeopensim3 PRIVATE ${OPENSIMSIMBODY_INCLUDE_DIRS} )
target_link_libraries( sconeopensim3 PUBLIC sconelib ${OPENSIMSIMBODY_LIBRARIES} )

# Require C++17 standard
set_target_properties(sconeopensim3 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_precompile_headers(sconeopensim3 PRIVATE <string> <vector> <algorithm> <memory> <limits> <fstream> <OpenSim/OpenSim.h>)
	file (GLOB_RECURSE PRECOMPILED_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cmake_pch.*)
	source_group("CMakePCH" FILES ${PRECOMPILED_HEADER_FILES})

	target_compile_definitions( sconeopensim3 PRIVATE SCONE_MODEL_OPENSIM_3_EXPORTS )
	target_compile_definitions( sconeopensim3 PRIVATE $<$<BOOL:${SCONE_ENABLE_PROFILER}>:SCONE_ENABLE_XO_PROFILING> )
	target_compile_options( sconeopensim3 PRIVATE "/MP" ) # multithreaded compilation on MSVC
endif()

source_group("" FILES ${MODEL_OPENSIM_3_FILES})
