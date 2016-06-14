set(MANAGER_FILES SconeManager.cpp SconeManager.h)

add_executable(sconeopt sconeopt.cpp ${MANAGER_FILES})

if(UNIX)
    set(REALTIME_LIB ncurses)
    if(NOT APPLE)
        list(APPEND REALTIME_LIB tinfo)
    endif()
endif()

target_link_libraries(sconeopt sconelib ${REALTIME_LIB})
