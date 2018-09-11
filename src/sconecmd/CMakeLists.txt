add_executable(sconecmd sconecmd.cpp)

set_property(TARGET sconecmd PROPERTY CXX_STANDARD 17)
set_property(TARGET sconecmd PROPERTY CXX_STANDARD_REQUIRED ON)

if(UNIX)
    set(REALTIME_LIB ncurses)
    if(NOT APPLE)
        list(APPEND REALTIME_LIB tinfo)
    endif()
endif()

target_link_libraries(sconecmd sconelib ${REALTIME_LIB})
