set(FILES
    main.cpp
	optimization_test.h
	propnode_test.h
	simulation_test.h
	)

add_executable(scone-unit-tests ${FILES})
set_target_properties(scone-unit-tests PROPERTIES
    PROJECT_LABEL "sconeunittests"
    OUTPUT_NAME sconeunittests)

target_link_libraries(scone-unit-tests sconelib xo)
