set(MODEL_OPENSIM_4_FILES
	BodyOpenSim4.cpp
	BodyOpenSim4.h
	DofOpenSim4.cpp
	DofOpenSim4.h
	JointOpenSim4.cpp
	JointOpenSim4.h
	ModelOpenSim4.cpp
	ModelOpenSim4.h
	MuscleOpenSim4.cpp
	MuscleOpenSim4.h
	SimulationOpenSim4.cpp
	SimulationOpenSim4.h
	ConstantForce.cpp
	ConstantForce.h
	simbody_tools.h
	platform.h
)

if (MSVC)
	add_compile_options( "/wd5040" )
endif()

find_package(OpenSim 4.0 REQUIRED CONFIG) # This uses the FindOpenSim.cmake file in build/cmake.

add_library( sconeopensim4 SHARED ${MODEL_OPENSIM_4_FILES} )
target_link_libraries( sconeopensim4 sconelib osimTools ${OPENSIMSIMBODY_LIBRARIES} )

# Require C++17 standard
set_target_properties(sconeopensim4 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_compile_definitions( sconeopensim4 PRIVATE SCONE_MODEL_OPENSIM_4_EXPORTS )
	target_compile_definitions( sconeopensim4 PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS )
	target_compile_definitions( sconeopensim4 PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_XO_PROFILING>" )
	add_compile_options("/Wd5040")
endif()

source_group("" FILES ${MODEL_OPENSIM_4_FILES})
