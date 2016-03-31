add_executable(sconeopt sconeopt.cpp)

if(UNIX AND NOT APPLE)
    set(REALTIME_LIB ncurses tinfo)
endif()

target_link_libraries(sconeopt sconelib ${REALTIME_LIB})
