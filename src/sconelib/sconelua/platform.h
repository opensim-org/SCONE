#pragma once

#if defined(_MSC_VER)
#	ifdef SCONE_LUA_EXPORTS
#		define SCONE_LUA_API __declspec(dllexport)
#	else
#		define SCONE_LUA_API __declspec(dllimport)
#	endif
#else
#	define SCONE_LUA_API
#endif
