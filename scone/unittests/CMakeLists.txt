set(FILES
    common.h
	platform.h
    main.cpp
	propnode_test.cpp
	simulation_test.cpp
	global_fixture.cpp
	)

add_executable(scone-unit-tests ${FILES})
set_target_properties(scone-unit-tests PROPERTIES
    PROJECT_LABEL "sconeunittests"
    OUTPUT_NAME sconeunittests)

target_link_libraries(scone-unit-tests sconelib)
