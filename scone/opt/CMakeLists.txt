set(API_FILES
    CmaOptimizer.cpp
    CmaOptimizer.h
    Objective.cpp
    Objective.h
    Optimizer.cpp
    Optimizer.h
    ParamInfo.cpp
    ParamInfo.h
    ParamSet.cpp
    ParamSet.h
    )
set(SYSTEM_FILES
    Factories.cpp
    Factories.h
    opt.cpp
    opt.h
    stdafx.cpp
    stdafx.h
    )
add_library(scone-opt # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${API_FILES} ${SYSTEM_FILES}
    )
target_link_libraries(scone-opt scone-core
    ${Shark_LIBRARIES} ${Boost_LIBRARIES})

set_target_properties(scone-opt PROPERTIES
    DEFINE_SYMBOL OPT_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL opt # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
source_group("API" FILES ${API_FILES})
source_group("System" FILES ${SYSTEM_FILES})