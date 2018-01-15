set(CORE_BASE_FILES
    Exception.h
    Factory.h
    HasData.h
    HasName.h
    HasSignature.cpp
    HasSignature.h
	Result.h
    )
set(CORE_MATH_FILES
    Vec3.h
    Quat.h
    Angle.h
    Delayer.h
    math.h
    Range.h
    Statistic.h
    TimedValue.h
    )
set(CORE_PROPNODE_FILES
    InitFromPropNode.h
    PropNode.cpp
    PropNode.h
    )
set(CORE_SYSTEM_FILES
    core.h
    )
set(CORE_TOOLS_FILES
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
    tools.cpp
    tools.h
    )

add_library(scone-core # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${CORE_BASE_FILES} ${CORE_MATH_FILES} ${CORE_PROPNODE_FILES}
    ${CORE_SYSTEM_FILES} ${CORE_TOOLS_FILES})
target_link_libraries(scone-core ${Boost_LIBRARIES})

set_target_properties(scone-core PROPERTIES
    DEFINE_SYMBOL CORE_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL core # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
source_group("Base" FILES ${CORE_BASE_FILES})
source_group("Math" FILES ${CORE_MATH_FILES})
source_group("PropNode" FILES ${CORE_PROPNODE_FILES})
source_group("System" FILES ${CORE_SYSTEM_FILES})
source_group("Tools" FILES ${CORE_TOOLS_FILES})

