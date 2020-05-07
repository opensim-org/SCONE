#pragma once

#pragma once

#include "platform.h"

#if defined SCONE_ENABLE_PROFILING
#	include "Profiler.h"
#	define SCONE_PROFILE_FUNCTION ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), __FUNCTION__ )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) ScopedProfile unique_scoped_function_profile( Profiler::GetGlobalInstance(), scope_name_arg )
#	define SCONE_PROFILE_START Profiler::GetGlobalInstance().Reset()
#	define SCONE_PROFILE_REPORT log::info( Profiler::GetGlobalInstance().GetReport() )
#elif defined SCONE_ENABLE_XO_PROFILING
#	include "PropNode.h"
#	include "xo/system/profiler.h"
#	define SCONE_PROFILE_FUNCTION xo::scope_profiler unique_scoped_profile( __FUNCTION__, xo::profiler::instance() )
#	define SCONE_PROFILE_SCOPE( scope_name_arg ) xo::scope_profiler unique_scoped_profile( scope_name_arg, xo::profiler::instance() )
#	define SCONE_PROFILE_START xo::profiler::instance().start()
#	define SCONE_PROFILE_REPORT xo::profiler::instance().log_results()
#else 
#	define SCONE_PROFILE_FUNCTION void()
#	define SCONE_PROFILE_SCOPE( scope_name_arg )
#	define SCONE_PROFILE_START void()
#	define SCONE_PROFILE_REPORT PropNode()
#endif
