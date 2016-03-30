set(FILES
    vis.h
    )
add_library(scone-vis SHARED ${FILES})

#target_link_libraries(scone-vis)

set_target_properties(scone-vis PROPERTIES
	LINKER_LANGUAGE CXX
    DEFINE_SYMBOL VIS_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL vis # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )
