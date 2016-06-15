add_executable(sconeopt sconeopt.cpp)

if(UNIX)
    set(REALTIME_LIB ncurses)
    if(NOT APPLE)
        list(APPEND REALTIME_LIB tinfo)
    endif()
endif()

target_link_libraries(sconeopt sconelib ${REALTIME_LIB})
