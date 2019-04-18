set(FILES
    main.cpp
	optimization_test.h
	simulation_test.h
	)

add_executable(sconeunittests ${FILES})

# Require C++17 standard
set_target_properties(sconeunittests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(sconeunittests sconelib xo)

if (UNIX)
    target_link_libraries(sconeunittests stdc++fs)
endif()

if (SCONE_OPENSIM_3)
	target_link_libraries(sconeunittests sconeopensim3)
	target_compile_definitions( sconeunittests PRIVATE SCONE_OPENSIM_3 )
endif()
if (SCONE_OPENSIM_4)
	target_link_libraries(sconeunittests sconeopensim4)
	target_compile_definitions( sconeunittests PRIVATE SCONE_OPENSIM_4 )
endif()

source_group("" FILES ${FILES})
