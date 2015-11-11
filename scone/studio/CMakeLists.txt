
if(${Qt5Widgets_FOUND})
    # http://doc.qt.io/qt-5/cmake-manual.html

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    
    set(FILES
        SconeManager.cpp
        SconeManager.h
        SconeStudio.cpp
        SconeStudio.h
        stdafx.cpp
        stdafx.h
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
        # Copy Qt dll's to the binary directory.
        set(QT5_LIBS Qt5Widgets Qt5Core Qt5Gui)
        foreach(qt5lib ${QT5_LIBS})
            add_custom_command(TARGET scone-studio POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Widgets_DIR}/../../../bin/${qt5lib}.dll
                                             ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>
                COMMAND ${CMAKE_COMMAND} -E copy ${Qt5Widgets_DIR}/../../../bin/${qt5lib}d.dll
                                             ${EXECUTABLE_OUTPUT_PATH}/$<CONFIGURATION>
                COMMENT "Copying Qt library ${qt5lib} to binary directory." VERBATIM
                                             )
        endforeach()
            
    endif()

endif()