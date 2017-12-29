set(FILES
    common.h
	platform.h
	global_fixture.cpp
    main.cpp
	optimization_test.cpp
	propnode_test.cpp
	simulation_test.cpp
	)

add_executable(scone-unit-tests ${FILES})
set_target_properties(scone-unit-tests PROPERTIES
    PROJECT_LABEL "sconeunittests"
    OUTPUT_NAME sconeunittests)

target_link_libraries(scone-unit-tests sconelib ${Boost_LIBRARIES} )
