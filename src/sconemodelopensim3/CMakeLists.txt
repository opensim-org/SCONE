
if(SCONE_OPENSIM_3)
	set(MODEL_OPENSIM_3_FILES
		BodySimbody.cpp
		BodySimbody.h
		DofSimbody.cpp
		DofSimbody.h
		JointSimbody.cpp
		JointSimbody.h
		ModelSimbody.cpp
		ModelSimbody.h
		MuscleSimbody.cpp
		MuscleSimbody.h
		SimulationSimbody.cpp
		SimulationSimbody.h
		ConstantForce.cpp
		ConstantForce.h
		simbody_tools.h
		platform.h
	)

	find_package( OpenSim 3.3 REQUIRED MODULE )
    include_directories( ${OPENSIMSIMBODY_INCLUDE_DIRS} )
	add_library( sconemodelopensim3 SHARED ${MODEL_OPENSIM_3_FILES} )
	target_link_libraries( sconemodelopensim3 sconelib ${OPENSIMSIMBODY_LIBRARIES} )

	if (MSVC)
		target_compile_definitions( sconemodelopensim3 PRIVATE SCONE_MODEL_OPENSIM_3_EXPORTS )
		target_compile_definitions( sconemodelopensim3 PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS )
		target_compile_definitions( sconemodelopensim3 PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_XO_PROFILING>" )
		add_compile_options("/Wd5040")
	endif()

	#source_group("" FILES ${MODEL_OPENSIM_3_FILES})
endif()
