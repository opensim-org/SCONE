file (GLOB SCONE_LUA_FILES "*.h" "*.cpp")

find_package(Lua 5.3.5 EXACT REQUIRED)

find_path( SOL_SINGLE_INCLUDE_DIR NAMES sol/sol.hpp )

include( PrecompiledHeaders )
use_precompiled_headers( SCONE_LUA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h" "stdafx.cpp" 256 )

add_library( sconelua SHARED ${SCONE_LUA_FILES} )
target_link_libraries( sconelua sconelib ${LUA_LIBRARY})
target_include_directories( sconelua PRIVATE ${LUA_INCLUDE_DIR} ${SOL_SINGLE_INCLUDE_DIR})
set_target_properties( sconelua PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_compile_definitions( sconelua PRIVATE SCONE_LUA_EXPORTS )
	target_compile_definitions( sconelua PRIVATE SCONE_ENABLE_XO_PROFILING )
endif()

source_group( "" FILES ${SCONE_LUA_FILES} )
