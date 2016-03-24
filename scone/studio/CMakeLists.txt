if(${Qt5Widgets_FOUND})
	# http://doc.qt.io/qt-5/cmake-manual.html

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	
	set(FILES
		SconeManager.cpp
		SconeManager.h
		SconeStudio.cpp
		SconeStudio.h
		main.cpp
		studio.rc
		)
	
	qt5_wrap_ui(UI_HEADERS SconeStudio.ui)
	qt5_add_resources(RESOURCES SconeStudio.qrc)

	add_executable(scone-studio ${FILES} ${UI_HEADERS} ${RESOURCES})

	set_target_properties(scone-studio PROPERTIES
		PROJECT_LABEL "sconestudio"
		OUTPUT_NAME sconestudio
		)

	target_link_libraries(scone-studio scone-cs Qt5::Widgets ${Boost_LIBRARIES})
	
	if(WIN32)
		# Set DLL target
		SET(DLL_TARGET_DIR ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>)
		add_custom_command(TARGET scone-studio POST_BUILD
			COMMENT "Updating DLLs" VERBATIM
		)

		# Copy all Boost DLLs to the binary directory.
		SET(BOOST_DLLS filesystem system thread date_time chrono serialization)
		foreach(boostdll ${BOOST_DLLS})
			add_custom_command(TARGET scone-studio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Boost_LIBRARY_DIRS}/boost_${boostdll}-vc120-mt-$<$<CONFIG:debug>:gd->${Boost_LIB_VERSION}.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		# Copy Qt DLLs to the binary directory.
		SET(QT5_LIBS Qt5Widgets Qt5Core Qt5Gui)
		foreach(qt5lib ${QT5_LIBS})
			add_custom_command(TARGET scone-studio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Widgets_DIR}/../../../bin/${qt5lib}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()
	
		# Copy all OpenSim DLLs
		FILE(GLOB OPENSIM_DLLS ${OPENSIM_INSTALL_DIR}/bin/*.dll)
		foreach(opensimdll ${OPENSIM_DLLS})
			add_custom_command(TARGET scone-studio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${opensimdll} ${DLL_TARGET_DIR}
			)
		endforeach()
	endif()
endif()
