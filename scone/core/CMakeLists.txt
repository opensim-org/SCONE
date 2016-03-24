set(BASE_FILES
    Exception.h
    Factory.h
    HasData.h
    HasName.h
    HasSignature.cpp
    HasSignature.h
	Result.h
    )
set(MATH_FILES
    Angle.h
    Delayer.h
    math.h
    Range.h
    Statistic.h
    TimedValue.h
    )
set(PROPNODE_FILES
    InitFromPropNode.h
    PropNode.cpp
    PropNode.h
    )
set(SYSTEM_FILES
    core.h
    stdafx.cpp
    stdafx.h
    )
set(TOOLS_FILES
    Log.cpp
    Log.h
    Profiler.cpp
    Profiler.h
    ResourceCache.h
    Storage.h
	StorageIo.h
    StringMap.h
    system.cpp
    system.h
    Timer.cpp
    Timer.h
    tools.cpp
    tools.h
    )
add_library(scone-core # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${BASE_FILES} ${MATH_FILES} ${PROPNODE_FILES}
    ${SYSTEM_FILES} ${TOOLS_FILES})
target_link_libraries(scone-core ${Boost_LIBRARIES})

set_target_properties(scone-core PROPERTIES
    DEFINE_SYMBOL CORE_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL core # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
source_group("Base" FILES ${BASE_FILES})
source_group("Math" FILES ${MATH_FILES})
source_group("PropNode" FILES ${PROPNODE_FILES})
source_group("System" FILES ${SYSTEM_FILES})
source_group("Tools" FILES ${TOOLS_FILES})

