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
		ModelVis.cpp
		ModelVis.h
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
		GaitAnalysis.h
		GaitAnalysis.cpp
		GaitPlot.h
		GaitPlot.cpp
		OptimizerTask.h
		OptimizerTask.cpp
		OptimizerTaskExternal.h
		OptimizerTaskExternal.cpp
		OptimizerTaskThreaded.h
		OptimizerTaskThreaded.cpp
		)
	
	set(QTFILES
		${QTFX_PATH}/qt_convert.h
		${QTFX_PATH}/qtfx.h
		${QTFX_PATH}/qtfx.cpp
		${QTFX_PATH}/QGroup.h
		${QTFX_PATH}/QGroup.cpp
		${QTFX_PATH}/QOsgViewer.h
		${QTFX_PATH}/QOsgViewer.cpp
		${QTFX_PATH}/osg_camera_man.h
		${QTFX_PATH}/osg_camera_man.cpp
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
		${QTFX_PATH}/QValueSlider.h
		${QTFX_PATH}/QValueSlider.cpp
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
		../../resources/studio-settings-schema.zml
		)

	INCLUDE_DIRECTORIES( ../../contrib/qcustomplot )
	INCLUDE_DIRECTORIES( ${QTFX_PATH} )

	qt5_wrap_ui(UI_HEADERS SconeStudio.ui ProgressDockWidget.ui SconeSettings.ui)
	qt5_add_resources(RESOURCES SconeStudio.qrc)

	SET( SOURCE_FILES ${STUDIOFILES} ${QTFILES} ${RESOURCES} )
	include( PrecompiledHeaders )
	use_precompiled_headers( SOURCE_FILES "stdafx.h" "stdafx.cpp" 256 )

	add_executable(sconestudio ${STUDIOFILES} ${QTFILES} ${PRECOMPILED_HEADER_FILES} ${UI_HEADERS} ${RESOURCEFILES} )

	target_compile_definitions(sconestudio PRIVATE QTFX_USE_QCUSTOMPLOT)

	qt5_use_modules( sconestudio Widgets OpenGL PrintSupport )

	set_target_properties(sconestudio PROPERTIES PROJECT_LABEL "sconestudio" OUTPUT_NAME sconestudio )

	# Require C++17 standard
	set_target_properties(sconestudio PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

	# setup OpenSceneGraph stuff
	FIND_PACKAGE( OpenSceneGraph REQUIRED
	  osg
	  osgWidget
	  osgQt
	  osgGA
	  osgViewer
	  osgUtil
	  osgDB
	  osgShadow
	  OpenThreads )

	# set target include libraries
	target_include_directories(sconestudio PRIVATE ${VIS_INCLUDE_DIR} ${OSG_INCLUDE_DIR} ${OSGQT_INCLUDE_DIR})

	target_link_libraries(sconestudio
		sconelib
		vis-osg
		${OSG_LIBRARIES}
		${OSGWIDGET_LIBRARIES}
		${OSGGA_LIBRARIES}
		${OSGVIEWER_LIBRARIES}
		${OSGQT_LIBRARIES}
		${OSGDB_LIBRARIES}
		${OSGSHADOW_LIBRARIES}
		${OPENTHREADS_LIBRARIES}
		)

	if (SCONE_OPENSIM_3)
		target_link_libraries(sconestudio sconeopensim3)
		target_compile_definitions( sconestudio PRIVATE SCONE_OPENSIM_3 )
	endif()

	if (SCONE_OPENSIM_4)
		target_link_libraries(sconestudio sconeopensim4)
		target_compile_definitions( sconestudio PRIVATE SCONE_OPENSIM_4 )
	endif()

	if (SCONE_HYFYDY)
		target_link_libraries(sconestudio sconehfd)
		target_compile_definitions(sconestudio PRIVATE SCONE_HYFYDY)
	endif()

	if (SCONE_LUA)
		target_link_libraries(sconestudio sconelua)
		target_compile_definitions( sconestudio PRIVATE SCONE_LUA )
	endif()

	target_compile_definitions( sconestudio PRIVATE SCONE_ENABLE_XO_PROFILING )

	if(MSVC)
		# no console in release mode
		set_target_properties( sconestudio PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS" )
		target_compile_options( sconestudio PRIVATE "/MP" ) # multithreaded compilation on MSVC

		source_group("Precompiled Header Files" FILES ${PRECOMPILED_HEADER_FILES} )
		source_group("Qt Files" FILES ${QTFILES} ${UI_HEADERS} ${RESOURCES} "${CMAKE_CURRENT_BINARY_DIR}/sconestudio_automoc.cpp" )
		source_group("Studio Files" FILES ${STUDIOFILES} )
		source_group("Resource Files" FILES ${RESOURCEFILES} )
	
		# copy dlls
		# get the VS version, the shabby cmake way.
		string(SUBSTRING ${CMAKE_VS_PLATFORM_TOOLSET} 1 3 VS_PLATFORM_TOOLSET_NUMBER)

		# Set DLL target
		SET(DLL_TARGET_DIR ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>)
		add_custom_command(TARGET sconestudio POST_BUILD
			COMMENT "Updating DLLs" VERBATIM
		)

		# Copy Qt DLLs to the binary directory.
		SET(QT5_LIBS Qt5Widgets Qt5Core Qt5Gui Qt5OpenGl Qt5PrintSupport )
		foreach(qt5lib ${QT5_LIBS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Widgets_DIR}/../../../bin/${qt5lib}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		add_custom_command(TARGET sconestudio POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${DLL_TARGET_DIR}/platforms")
		add_custom_command(TARGET sconestudio POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Widgets_DIR}/../../../plugins/platforms/qwindows$<$<CONFIG:debug>:d>.dll "${DLL_TARGET_DIR}/platforms"
		)
	
		# Copy all OpenSim3 DLLs
		if (SCONE_OPENSIM_3)
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
		endif()

		# Copy OSG DLLs
		# get OSG_INSTALL_DIR
		get_filename_component( OSG_INSTALL_DIR ${OSG_INCLUDE_DIR} DIRECTORY )
		set(OSG_DLL_DIR ${OSG_INSTALL_DIR}/bin CACHE PATH "Installation directory of OpenSceneGraph")

		set(OSG_DLL_PREFIX "osg131-" CACHE STRING "Prefix of OpenSceneGraph DLLs. Yes, you need to set this by hand :-(" )
		SET(OSG_DLLS
			${OSG_DLL_PREFIX}osg ${OSG_DLL_PREFIX}osgwidget ${OSG_DLL_PREFIX}osgqt
			${OSG_DLL_PREFIX}osgga ${OSG_DLL_PREFIX}osgviewer ${OSG_DLL_PREFIX}osgutil
			${OSG_DLL_PREFIX}osgtext ${OSG_DLL_PREFIX}osgdb ${OSG_DLL_PREFIX}osgshadow
			ot20-OpenThreads zlib libpng)
		foreach(osgdll ${OSG_DLLS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OSG_DLL_DIR}/${osgdll}$<$<CONFIG:debug>:d>.dll ${DLL_TARGET_DIR}
			)
		endforeach()

		# Copy osg png plugin
		set(OSG_PLUGINS_FOLDER "osgPlugins-3.4.1" CACHE STRING "Prefix of OpenSceneGraph Plugins folder. Yes, you need to set this by hand :-(" )
		add_custom_command(TARGET sconestudio POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${DLL_TARGET_DIR}/${OSG_PLUGINS_FOLDER}" )
		set(OSG_PLUGINS osgdb_png osgdb_obj osgdb_stl)
		foreach(osgplugin ${OSG_PLUGINS})
			add_custom_command(TARGET sconestudio POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OSG_DLL_DIR}/${OSG_PLUGINS_FOLDER}/${osgplugin}$<$<CONFIG:debug>:d>.dll "${DLL_TARGET_DIR}/${OSG_PLUGINS_FOLDER}"
			)
		endforeach()
	endif()
endif()
