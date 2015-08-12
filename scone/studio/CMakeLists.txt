
if(${Qt5Core_FOUND})
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

endif()