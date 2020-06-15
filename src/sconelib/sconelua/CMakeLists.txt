file (GLOB SCONE_LUA_FILES "*.h" "*.cpp")

# liblua is build as part of SCONE -- no need for find_package (which doesn't work for debug builds)
# find_package(Lua 5.3.5 EXACT REQUIRED)

find_path( SOL_SINGLE_INCLUDE_DIR NAMES sol/sol.hpp )

add_library( sconelua SHARED ${SCONE_LUA_FILES} )
target_link_libraries( sconelua sconelib liblua )
target_include_directories( sconelua PRIVATE ${LUA_INCLUDE_DIR} ${SOL_SINGLE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties( sconelua PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_precompile_headers(sconelua PRIVATE <string> <vector> <algorithm> <memory> <limits>
		[["sol_config.h"]]
	)
	file (GLOB_RECURSE PRECOMPILED_HEADER_FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cmake_pch.*)
	source_group("CMakePCH" FILES ${PRECOMPILED_HEADER_FILES})

	target_compile_definitions( sconelua PRIVATE SCONE_LUA_EXPORTS )
	target_compile_definitions( sconelua PRIVATE $<$<BOOL:${SCONE_ENABLE_PROFILER}>:SCONE_ENABLE_XO_PROFILING> )
	#target_compile_options( sconelua PRIVATE "/MP" ) # multithreaded compilation on MSVC
	target_compile_options( sconelua PRIVATE "/bigobj" ) # needed to prevent C1128
endif()

source_group( "" FILES ${SCONE_LUA_FILES} )
