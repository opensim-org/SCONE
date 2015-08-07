
set(SOURCES
    Exception.h
    Factory.h
    HasData.h
    HasName.cpp
    HasName.h
    HasSignature.cpp
    HasSignature.h
    Angle.h
    Delayer.h
    math.h
    Quat.cpp
    Quat.h
    Range.h
    Statistic.h
    TimedValue.h
    Vec3.cpp
    Vec3.h
    InitFromPropNode.h
    PropNode.cpp
    PropNode.h
    core.h
    stdafx.cpp
    stdafx.h
    Log.cpp
    Log.h
    #    Profiler.cpp
    #    Profiler.h
    ResourceCache.h
    Storage.h
    StringMap.h
    #    system.cpp
    #    system.h
    #    Timer.cpp
    #    Timer.h
    tools.cpp
    tools.h
    )

add_library(scone-core ${SOURCES})
target_link_libraries(scone-core ${Boost_LIBRARIES})

