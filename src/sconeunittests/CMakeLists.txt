set(FILES
    main.cpp
	optimization_test.cpp
	tutorial_test.cpp
	)

add_executable(sconeunittests ${FILES})

# Require C++17 standard
set_target_properties(sconeunittests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# If using GCC before version 9, add library for filesystem
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        set(FILESYSTEM_LIB stdc++fs)
    endif()
endif()

target_link_libraries(sconeunittests sconelib xo ${FILESYSTEM_LIB})
if (MSVC)
	target_compile_options(sconeunittests PRIVATE "/MP" ) # multithreaded compilation on MSVC
endif()

if (SCONE_OPENSIM_3)
	target_link_libraries(sconeunittests sconeopensim3)
	target_compile_definitions(sconeunittests PRIVATE SCONE_OPENSIM_3)
endif()
if (SCONE_OPENSIM_4)
	target_link_libraries(sconeunittests sconeopensim4)
	target_compile_definitions(sconeunittests PRIVATE SCONE_OPENSIM_4)
endif()
if (SCONE_LUA)
	target_link_libraries(sconeunittests sconelua)
	target_compile_definitions(sconeunittests PRIVATE SCONE_LUA)
endif()

source_group("" FILES ${FILES})
