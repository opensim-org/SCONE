set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(FILES
	SconeManager.cpp
	SconeManager.h
	main.cpp
	)

add_executable(scone-manager ${FILES})

set_target_properties(scone-manager PROPERTIES
	PROJECT_LABEL "sconemanager"
	OUTPUT_NAME sconemanager
	)

target_link_libraries(scone-manager sconelib ${Boost_LIBRARIES})

if(WIN32)
	# Set DLL target
	SET(DLL_TARGET_DIR ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>)
	add_custom_command(TARGET scone-manager POST_BUILD
		COMMENT "Updating DLLs" VERBATIM
	)

	# Copy all Boost DLLs to the binary directory.
	SET(BOOST_DLLS filesystem system thread date_time chrono serialization timer unit_test_framework)
	foreach(boostdll ${BOOST_DLLS})
		add_custom_command(TARGET scone-manager POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Boost_LIBRARY_DIRS}/boost_${boostdll}-vc120-mt-$<$<CONFIG:debug>:gd->${Boost_LIB_VERSION}.dll ${DLL_TARGET_DIR}
		)
	endforeach()

	# Copy all OpenSim DLLs
	FILE(GLOB OPENSIM_DLLS ${OPENSIM_INSTALL_DIR}/bin/*.dll)
	foreach(opensimdll ${OPENSIM_DLLS})
		add_custom_command(TARGET scone-manager POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${opensimdll} ${DLL_TARGET_DIR}
		)
	endforeach()
endif()
