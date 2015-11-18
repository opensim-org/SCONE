

add_executable(sconeopt stdafx.h stdafx.cpp sconeopt.cpp)
if(UNIX AND NOT APPLE)
    set(REALTIME_LIB ncurses tinfo)
endif()
target_link_libraries(sconeopt scone-cs ${REALTIME_LIB})
