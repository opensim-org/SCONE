if(${Qt5Widgets_FOUND})
	# http://doc.qt.io/qt-5/cmake-manual.html
	
	# set CMP0043 to get rid of cryptic CMake warnings
	cmake_policy( SET CMP0043 OLD )

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	
	set(STUDIOFILES
		SconeManager.cpp
		SconeManager.h
		SconeStudio.cpp
		SconeStudio.h
		OsgScene.h
		OsgScene.cpp
		main.cpp
		)
	
	set(QTFILES
		qosgviewer.h
		main.cpp
		studio.rc
		)

	qt5_wrap_ui(UI_HEADERS SconeStudio.ui)
	qt5_add_resources(RESOURCES SconeStudio.qrc)

	add_executable(sconestudio ${STUDIOFILES} ${QTFILES} ${UI_HEADERS} ${RESOURCES})

	qt5_use_modules( sconestudio Widgets OpenGL )
	
	set_target_properties(sconestudio PROPERTIES
		PROJECT_LABEL "sconestudio"
		OUTPUT_NAME sconestudio
		)
		
	# setup OpenSceneGraph stuff
	set(OSG_DIR "" CACHE PATH "Installation directory of OpenSceneGraph")
	FIND_PACKAGE( osg )
	FIND_PACKAGE( osgwidget )
	FIND_PACKAGE( osgqt )
	FIND_PACKAGE( osgga )
	FIND_PACKAGE( osgviewer )
	FIND_PACKAGE( osgutil )
	FIND_PACKAGE( osgdb )
	FIND_PACKAGE( OpenThreads )
	INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})

	target_link_libraries(sconestudio
		sconelib
#		Qt5::Widgets
		${Boost_LIBRARIES}
		${OSG_LIBRARIES}
		${OSGWIDGET_LIBRARIES}
		${OSGGA_LIBRARIES}
		${OSGVIEWER_LIBRARIES}
		${OSGQT_LIBRARIES}
		${OSGDB_LIBRARIES}
		${OPENTHREADS_LIBRARIES}
		)

	source_group("Qt Files" FILES ${QTFILES} ${UI_HEADERS} ${RESOURCES} SconeStudio.ui SconeStudio.qrc sconestudio_automoc.cpp)
	source_group("Studio Files" FILES ${STUDIOFILES})
	
	if(WIN32)
		# Set DLL target
		SET(DLL_TARGET_DIR ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>)
		add_custom_command(TARGET sconestudio POST_BUILD
			COMMENT "Updating DLLs" VERBATIM
		)

		# Copy all Boost DLLs to the binary directory.
		SET(BOOST_DLLS filesystem system thread date_time chrono serialization timer unit_test_framework)
		foreach(boostdll ${BOOST_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Boost_LIBRARY_DIRS}/boost_${boostdll}-vc120-mt-$<$<CONFIG:debug>:gd->${Boost_LIB_VERSION}.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		# Copy Qt DLLs to the binary directory.
		SET(QT5_LIBS Qt5Widgets Qt5Core Qt5Gui)
		foreach(qt5lib ${QT5_LIBS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Widgets_DIR}/../../../bin/${qt5lib}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()
	
		# Copy all OpenSim DLLs
		FILE(GLOB OPENSIM_DLLS ${OPENSIM_INSTALL_DIR}/bin/*.dll)
		foreach(opensimdll ${OPENSIM_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${opensimdll} ${DLL_TARGET_DIR}
			)
		endforeach()
	endif()
endif()
