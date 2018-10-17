if(${Qt5Widgets_FOUND})
	# http://doc.qt.io/qt-5/cmake-manual.html
	
	# set CMP0043 to get rid of cryptic CMake warnings
	# cmake_policy( SET CMP0043 OLD )
	cmake_policy( SET CMP0071 OLD )

	set(CMAKE_AUTOMOC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	FIND_PATH(QTFX_PATH QOsgViewer.h "../../submodules/qtfx/qtfx")
	
	set(STUDIOFILES
		main.cpp
		StudioModel.cpp
		StudioModel.h
		studio_config.h
		SconeStudio.cpp
		SconeStudio.h
		ProgressDockWidget.h
		ProgressDockWidget.cpp
		SettingsEditor.h
		SettingsEditor.cpp
		StudioSettings.h
		StudioSettings.cpp
		SconeStorageDataModel.h
		SconeStorageDataModel.cpp
		ResultsFileSystemModel.h
		ResultsFileSystemModel.cpp
		)
	
	set(QTFILES
		${QTFX_PATH}/qt_convert.h
		${QTFX_PATH}/qtfx.h
		${QTFX_PATH}/qtfx.cpp
		${QTFX_PATH}/QGroup.h
		${QTFX_PATH}/QGroup.cpp
		${QTFX_PATH}/QOsgViewer.cpp
		${QTFX_PATH}/QOsgViewer.h
		${QTFX_PATH}/QOsgViewer.cpp
		${QTFX_PATH}/QCodeEditor.h
		${QTFX_PATH}/QCodeEditor.cpp
		${QTFX_PATH}/QCodeHighlighter.h
		${QTFX_PATH}/QCodeHighlighter.cpp
		${QTFX_PATH}/QPlayControl.h
		${QTFX_PATH}/QPlayControl.cpp
		${QTFX_PATH}/QFileBrowser.h
		${QTFX_PATH}/QFileBrowser.cpp
		${QTFX_PATH}/QLogSink.h
		${QTFX_PATH}/QLogSink.cpp
		${QTFX_PATH}/QDataAnalysisView.h
		${QTFX_PATH}/QDataAnalysisView.cpp
		${QTFX_PATH}/QDataAnalysisModel.h
		${QTFX_PATH}/QCompositeMainWindow.h
		${QTFX_PATH}/QCompositeMainWindow.cpp
		${QTFX_PATH}/QPropNodeItemModel.h
		${QTFX_PATH}/QPropNodeItemModel.cpp
		${QTFX_PATH}/QSettingsItemModel.h
		${QTFX_PATH}/QSettingsItemModel.cpp
		../../contrib/qcustomplot/qcustomplot.h
		../../contrib/qcustomplot/qcustomplot.cpp
		)
	
	set(RESOURCEFILES
		studio.rc
		SconeStudio.qrc 
		SconeStudio.ui
		ProgressDockWidget.ui
		SconeSettings.ui
		)

	INCLUDE_DIRECTORIES( ../../contrib/qcustomplot )
	INCLUDE_DIRECTORIES( ${QTFX_PATH} )

	qt5_wrap_ui(UI_HEADERS SconeStudio.ui ProgressDockWidget.ui SconeSettings.ui)
	qt5_add_resources(RESOURCES SconeStudio.qrc)

	SET( SOURCE_FILES ${STUDIOFILES} ${QTFILES} ${RESOURCES} )
	include( PrecompiledHeaders )
	use_precompiled_headers( SOURCE_FILES "stdafx.h" "stdafx.cpp" 256 )

	add_executable(sconestudio ${STUDIOFILES} ${QTFILES} ${PRECOMPILED_HEADER_FILES} ${UI_HEADERS} ${RESOURCEFILES} )

	set_property(TARGET sconestudio PROPERTY CXX_STANDARD 17)
	set_property(TARGET sconestudio PROPERTY CXX_STANDARD_REQUIRED ON)

	target_compile_definitions(sconestudio PRIVATE QTFX_USE_QCUSTOMPLOT)

	qt5_use_modules( sconestudio Widgets OpenGL PrintSupport )

	set_target_properties(sconestudio PROPERTIES PROJECT_LABEL "sconestudio" OUTPUT_NAME sconestudio )

	# setup OpenSceneGraph stuff
	set(OSG_DIR "" CACHE PATH "Installation directory of OpenSceneGraph")
	FIND_PACKAGE( osg )
	FIND_PACKAGE( osgwidget )
	FIND_PACKAGE( osgqt )
	FIND_PACKAGE( osgga )
	FIND_PACKAGE( osgviewer )
	FIND_PACKAGE( osgutil )
	FIND_PACKAGE( osgdb )
	FIND_PACKAGE( osgshadow )
	FIND_PACKAGE( OpenThreads )
	INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})

	target_link_libraries(sconestudio
		sconelib
		simvis
#		Qt5::Widgets
		${OSG_LIBRARIES}
		${OSGWIDGET_LIBRARIES}
		${OSGGA_LIBRARIES}
		${OSGVIEWER_LIBRARIES}
		${OSGQT_LIBRARIES}
		${OSGDB_LIBRARIES}
		${OSGSHADOW_LIBRARIES}
		${OPENTHREADS_LIBRARIES}
		)
	
	# no console in release mode
	set_target_properties( sconestudio PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS" )
	target_compile_definitions( sconestudio PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_XO_PROFILING>" )

	source_group("Precompiled Header Files" FILES ${PRECOMPILED_HEADER_FILES} )
	source_group("Qt Files" FILES ${QTFILES} ${UI_HEADERS} ${RESOURCES} "${CMAKE_CURRENT_BINARY_DIR}/sconestudio_automoc.cpp" )
	source_group("Studio Files" FILES ${STUDIOFILES} )
	source_group("Resource Files" FILES ${RESOURCEFILES} )
	
	# Try to add simvis
	INCLUDE_DIRECTORIES(${SIMVIS_INCLUDE_DIR})

	if (MSVC)
		add_custom_command(TARGET sconestudio POST_BUILD COMMAND "${CMAKE_SOURCE_DIR}/install.bat" )
	endif()
	
	if(MSVC)
		# copy dlls
		# get the VS version, the shabby cmake way.
		string(SUBSTRING ${CMAKE_VS_PLATFORM_TOOLSET} 1 3 VS_PLATFORM_TOOLSET_NUMBER)

		# Set DLL target
		SET(DLL_TARGET_DIR ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>)
		add_custom_command(TARGET sconestudio POST_BUILD
			COMMENT "Updating DLLs" VERBATIM
		)

		# Copy Qt DLLs to the binary directory.
		SET(QT5_LIBS Qt5Widgets Qt5Core Qt5Gui Qt5OpenGl Qt5PrintSupport)
		foreach(qt5lib ${QT5_LIBS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Widgets_DIR}/../../../bin/${qt5lib}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()
	
		# Copy all OpenSim DLLs
		SET(OPENSIM_DLLS osimTools osimAnalyses osimActuators osimSimulation osimLepton osimCommon SimTKsimbody SimTKmath SimTKcommon)
		foreach(opensimdll ${OPENSIM_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENSIM_INSTALL_DIR}/bin/${opensimdll}$<$<CONFIG:debug>:_d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		# Copy extra opensim files
		SET(OPENSIM_EXTRA_DLLS libblas liblapack libgcc_s_sjlj-1 libgfortran-3 libquadmath-0 pthreadVC2_x64)
		foreach(opensimdll ${OPENSIM_EXTRA_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENSIM_INSTALL_DIR}/bin/${opensimdll}.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		# Copy OSG DLLs
		SET(OSG_DLLS osg130-osg osg130-osgwidget osg130-osgqt osg130-osgga osg130-osgviewer osg130-osgutil osg130-osgtext osg130-osgdb osg130-osgshadow ot20-OpenThreads)
		foreach(osgdll ${OSG_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OSG_DIR}/bin/${osgdll}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()

	endif()
endif()
