add_executable(sconecmd sconecmd.cpp)

set_target_properties(sconecmd PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories(sconecmd PRIVATE ${TCLAP_DIR} )

target_link_libraries(sconecmd sconelib)

if (MSVC)
	target_precompile_headers(sconecmd PRIVATE <string> <vector> <algorithm> <memory> <limits> <fstream>)
	file (GLOB_RECURSE PRECOMPILED_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cmake_pch.*)
	source_group("CMakePCH" FILES ${PRECOMPILED_HEADER_FILES})
	source_group("" FILES sconecmd.cpp)
endif()

if (SCONE_OPENSIM_3)
	target_link_libraries(sconecmd sconeopensim3)
	target_compile_definitions(sconecmd PRIVATE SCONE_OPENSIM_3)
endif()

if (SCONE_OPENSIM_4)
	target_link_libraries(sconecmd sconeopensim4)
	target_compile_definitions(sconecmd PRIVATE SCONE_OPENSIM_4)
endif()

if (SCONE_HYFYDY)
	target_link_libraries(sconecmd sconehfd)
	target_compile_definitions(sconecmd PRIVATE SCONE_HYFYDY)
endif()

if (SCONE_LUA)
	target_link_libraries(sconecmd sconelua)
	target_compile_definitions(sconecmd PRIVATE SCONE_LUA)
endif()
