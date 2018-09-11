set(FILES
    main.cpp
	optimization_test.h
	propnode_test.h
	simulation_test.h
	)

add_executable(scone-unit-tests ${FILES})

set_property(TARGET scone-unit-tests PROPERTY CXX_STANDARD 17)
set_property(TARGET scone-unit-tests PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(scone-unit-tests PROPERTIES
    PROJECT_LABEL "sconeunittests"
    OUTPUT_NAME sconeunittests)

target_link_libraries(scone-unit-tests sconelib xo)
