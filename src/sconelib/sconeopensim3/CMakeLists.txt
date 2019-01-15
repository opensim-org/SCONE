if(SCONE_OPENSIM_3)
	set(MODEL_OPENSIM_3_FILES
		BodyOpenSim3.cpp
		BodyOpenSim3.h
		DofOpenSim3.cpp
		DofOpenSim3.h
		JointOpenSim3.cpp
		JointOpenSim3.h
		ModelOpenSim3.cpp
		ModelOpenSim3.h
		MuscleOpenSim3.cpp
		MuscleOpenSim3.h
		SimulationOpenSim3.cpp
		SimulationOpenSim3.h
		ConstantForce.cpp
		ConstantForce.h
		simbody_tools.h
		platform.h
	)

	if (MSVC)
		add_compile_options( "/wd5040" )
	endif()
	
	find_package( OpenSim 3.3 REQUIRED MODULE )
    include_directories( ${OPENSIMSIMBODY_INCLUDE_DIRS} )
	add_library( sconeopensim3 SHARED ${MODEL_OPENSIM_3_FILES} )
	target_link_libraries( sconeopensim3 sconelib ${OPENSIMSIMBODY_LIBRARIES} )

	if (MSVC)
		target_compile_definitions( sconeopensim3 PRIVATE SCONE_MODEL_OPENSIM_3_EXPORTS )
		target_compile_definitions( sconeopensim3 PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS )
		target_compile_definitions( sconeopensim3 PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_XO_PROFILING>" )
	endif()

	#source_group("" FILES ${MODEL_OPENSIM_3_FILES})
endif()
